#include "Game.hpp"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <imgui.h>
#include <Logger.hpp>
#include <exception>
#include <fstream>
#include <glm/glm.hpp>
#include <memory>
#include <stdexcept>
#include "ECS.hpp"
#include "imgui_impl_sdl.h"
#include "imgui_impl_sdlrenderer.h"
#include "LevelLoader.hpp"
using namespace std::string_literals;
using glm::vec2;

using namespace constants;

Game::Game()
{
    registry = std::make_shared<Registry>();
    asset_store = std::make_shared<AssetStore>();
    event_bus = std::make_shared<EventBus>();
}

void Game::init()
{
    lua.open_libraries(sol::lib::base, sol::lib::package, sol::lib::os, sol::lib::math);
    lua.script_file("./scripts/config.lua");
    config = lua["config"];

    if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
    {
        Logger::error("SDL initialization failed."s);
        return;
    }

    if (TTF_Init() != 0)
    {
        Logger::error("TTF initialization failed."s);
        return;
    }

    SDL_DisplayMode displayMode;
    SDL_GetCurrentDisplayMode(0, &displayMode);

    window =
        SDL_CreateWindow(NULL, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                         window_width, window_height, SDL_WINDOW_BORDERLESS);

    if (!window)
    {
        Logger::error("Creating SDL window failed."s);
        return;
    }

    std::string window_title = config["title"];
    SDL_SetWindowTitle(window, window_title.c_str());

    // -1 means default renderer
    renderer = SDL_CreateRenderer(
        window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    if (!renderer)
    {
        Logger::error("Creating renderer failed."s);
        return;
    }

    // init imgui
    ImGui::CreateContext();
    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    // Setup Platform/Renderer backends
    ImGui_ImplSDL2_InitForSDLRenderer(window, renderer);
    ImGui_ImplSDLRenderer_Init(renderer);

    // init camera
    camera.x = 0;
    camera.y = 0;
    camera.w = displayMode.w;
    camera.h = displayMode.h;

    SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);

    registry->add_system<MovementSystem>();
    registry->add_system<RenderSystem>();
    registry->add_system<AnimationSystem>();
    registry->add_system<CollisionSystem>();
    registry->add_system<RenderColliderSystem>();
    registry->add_system<DamageSystem>();
    registry->add_system<KeyboardControlSystem>();
    registry->add_system<CameraMovementSystem>();
    registry->add_system<ProjectileEmitSystem>();
    registry->add_system<ProjectileLifecycleSystem>();
    registry->add_system<RenderTextSystem>();
    registry->add_system<RenderHealthSystem>();
    registry->add_system<RenderGuiSystem>();
    registry->add_system<MouseControlSystem>();
    registry->add_system<ScriptSystem>();

    registry->get_system<ScriptSystem>().create_lua_bindings(lua);
    registry->get_system<MovementSystem>().subscribe_events(event_bus);
    registry->get_system<DamageSystem>().subscribe_events(event_bus);
    registry->get_system<KeyboardControlSystem>().subscribe_events(event_bus);
    registry->get_system<ProjectileEmitSystem>().subscribe_events(event_bus);

    running = true;
}

void Game::load_level(int level)
{

    LevelLoader level_loader{registry, asset_store, renderer};
    level_loader.load(lua, level);
}

void Game::setup()
{
    load_level(1);
    Entity label = registry->create_entity();
    SDL_Color color = {0, 255, 0};
    label.add_component<TextComponent>(vec2(window_width / 2 - 40, 10),
                                       "Chopper 1.0", "main-font", color);
}

void Game::run()
{
    while (running)
    {
        process_input();
        update();
        render();
    }
}

void Game::destroy()
{
    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    SDL_Quit();
}

void Game::process_input()
{
    SDL_Event sdlEvent;
    while (SDL_PollEvent(&sdlEvent))
    {
        // ImGui SDL input
        ImGui_ImplSDL2_ProcessEvent(&sdlEvent);
        ImGuiIO &io = ImGui::GetIO();
        int mouse_x, mouse_y;
        const int buttons = SDL_GetMouseState(&mouse_x, &mouse_y);
        io.MousePos = ImVec2(static_cast<float>(mouse_x), static_cast<float>(mouse_y));
        io.MouseDown[0] = buttons & SDL_BUTTON(SDL_BUTTON_LEFT);
        io.MouseDown[1] = buttons & SDL_BUTTON(SDL_BUTTON_RIGHT);

        // handle core SDL events
        switch (sdlEvent.type)
        {
        case SDL_QUIT:
            running = false;
            break;
        case SDL_KEYDOWN:
            event_bus->emit<KeyPressedEvent>(sdlEvent.key.keysym.sym);
            if (sdlEvent.key.keysym.sym == SDLK_SLASH)
            {
                debug = !debug;
            }
            if (sdlEvent.key.keysym.sym == SDLK_ESCAPE)
            {
                running = false;
            }
            if (sdlEvent.key.keysym.sym == SDLK_F1)
            {
                show_gui = !show_gui;
            }
            break;
        case SDL_MOUSEBUTTONDOWN:
            if (sdlEvent.button.button == SDL_BUTTON_LEFT)
            {
                event_bus->emit<MouseClickedEvent>(sdlEvent.button.button);
            }
        }
    }
}

void Game::update()
{
    auto current_ticks = SDL_GetTicks();
    auto time_to_wait = constants::TICKS_PER_FRAME - (current_ticks - cum_ticks);
    // delta time
    float dt = (current_ticks - cum_ticks) / 1000.0f;
    registry->update();

    registry->get_system<MovementSystem>().update(dt);
    registry->get_system<AnimationSystem>().update();
    registry->get_system<CollisionSystem>().update(event_bus);
    registry->get_system<DamageSystem>().update();
    registry->get_system<CameraMovementSystem>().update(camera);
    registry->get_system<ProjectileEmitSystem>().update(registry);
    registry->get_system<ProjectileLifecycleSystem>().update();
    registry->get_system<ScriptSystem>().update(dt, SDL_GetTicks());

    if (time_to_wait > 0 && time_to_wait < constants::TICKS_PER_FRAME)
    {
        SDL_Delay(time_to_wait);
    }

    cum_ticks = SDL_GetTicks();
}

void Game::render()
{
    SDL_RenderClear(renderer);
    registry->get_system<RenderSystem>().update(renderer, asset_store, camera);
    registry->get_system<RenderTextSystem>().update(renderer, asset_store,
                                                    camera);
    registry->get_system<RenderHealthSystem>().update(renderer, asset_store,
                                                      camera);
    if (debug)
    {
        registry->get_system<RenderColliderSystem>().update(renderer, camera);
    }
    if (show_gui)
    {
        registry->get_system<RenderGuiSystem>().update(registry, camera);
    }
    SDL_RenderPresent(renderer);
}
