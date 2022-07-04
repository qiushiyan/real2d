#include "Game.hpp"
#include "ECS.hpp"
#include <Logger.hpp>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <glm/glm.hpp>
#include <exception>
#include <stdexcept>
#include <memory>
#include <fstream>
using namespace std::string_literals;
using glm::vec2;

Game::Game()
{
    registry = std::make_unique<Registry>();
    asset_store = std::make_unique<AssetStore>();
    event_bus = std::make_shared<EventBus>();
}

void Game::init()
{
    using namespace constants;
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
    {
        Logger::error("SDL initialization failed."s);
        return;
    }

    SDL_DisplayMode displayMode;
    SDL_GetCurrentDisplayMode(0, &displayMode);

    window = SDL_CreateWindow(NULL, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, window_width, window_height, SDL_WINDOW_BORDERLESS);

    if (!window)
    {
        Logger::error("Creating SDL window failed."s);
        return;
    }

    // -1 means default renderer
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    if (!renderer)
    {
        Logger::error("Creating renderer failed."s);
        return;
    }

    // init camera
    camera.x = 0;
    camera.y = 0;
    camera.w = displayMode.w;
    camera.h = displayMode.h;

    SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);

    running = true;
}

void Game::load_level(int level)
{
    using namespace constants;
    // add systems
    registry->add_system<MovementSystem>();
    registry->add_system<RenderSystem>();
    registry->add_system<AnimationSystem>();
    registry->add_system<CollisionSystem>();
    registry->add_system<RenderColliderSystem>();
    registry->add_system<DamageSystem>();
    registry->add_system<KeyboardControlSystem>();
    registry->add_system<CameraMovementSystem>();

    registry->get_system<DamageSystem>().subscribe_events(event_bus);
    registry->get_system<KeyboardControlSystem>().subscribe_events(event_bus);
    // add textures
    asset_store->add_texture(renderer, "radar-image", "../assets/images/radar.png");
    asset_store->add_texture(renderer, "tilemap-image", "../assets/tilemaps/jungle.png");
    asset_store->add_texture(renderer, "chopper-image", "../assets/images/chopper-spritesheet.png");
    asset_store->add_texture(renderer, "tank-image-left", "../assets/images/tank-tiger-left.png");
    asset_store->add_texture(renderer, "tank-image-right", "../assets/images/tank-tiger-right.png");

    // test animation
    auto chopper = registry->create_entity();
    chopper.add_component<TransformComponent>(vec2(10, 20), vec2(1, 1), 0.0);
    chopper.add_component<RigidBodyComponent>(vec2(100, 10));
    chopper.add_component<SpriteComponent>("chopper-image", tile_size, tile_size, 2, false, 0, tile_size);
    chopper.add_component<AnimationComponent>(2, 12, true);
    chopper.add_component<KeyboardControlComponent>(vec2(0, -100), vec2(100, 0), vec2(0, 100), vec2(-100, 0));
    chopper.add_component<CameraFollowComponent>();

    // test fixed entity
    auto radar = registry->create_entity();
    radar.add_component<TransformComponent>(vec2(map_width - tile_size, tile_size / 4), vec2(1, 1), 0.0);
    radar.add_component<SpriteComponent>("radar-image", 64, 64, 1, true);
    radar.add_component<AnimationComponent>(8, 4, true);

    // test collision
    auto tank1 = registry->create_entity();
    tank1.add_component<TransformComponent>(vec2(30, 30), vec2(1, 1), 0.0);
    tank1.add_component<RigidBodyComponent>(vec2(50, 0));
    tank1.add_component<SpriteComponent>("tank-image-right", tile_size, tile_size, 2);
    tank1.add_component<BoxColliderComponent>(tile_size, tile_size);

    auto tank2 = registry->create_entity();
    tank2.add_component<TransformComponent>(vec2(400, 30), vec2(1, 1), 0.0);
    tank2.add_component<RigidBodyComponent>(vec2(-50, 0));
    tank2.add_component<SpriteComponent>("tank-image-left", tile_size, tile_size, 2);
    tank2.add_component<BoxColliderComponent>(tile_size, tile_size, vec2(5));

    // load tilemap and create entities
    std::fstream map_file;
    map_file.open("./assets/tilemaps/jungle.map");

    if (!map_file.is_open())
    {
        Logger::error("Failed to open map file."s);
    }

    for (int y = 0; y < map_rows; y++)
    {
        for (int x = 0; x < map_cols; x++)
        {
            char ch;
            map_file.get(ch);
            int src_rect_y = std::atoi(&ch) * tile_size;
            map_file.get(ch);
            int src_rect_x = std::atoi(&ch) * tile_size;
            map_file.ignore();

            Entity tile = registry->create_entity();
            tile.add_component<TransformComponent>(vec2(x * (tile_scale * tile_size), y * (tile_scale * tile_size)), vec2(tile_scale, tile_scale), 0.0);
            tile.add_component<SpriteComponent>("tilemap-image", tile_size, tile_size, 0, false, src_rect_x, src_rect_y);
        }
    }
    map_file.close();
}

void Game::setup()
{
    load_level(1);
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
            break;
        }
    }
}

void Game::update()
{
    // event_bus->reset();
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

    if (time_to_wait > 0 && time_to_wait < constants::TICKS_PER_FRAME)
    {
        SDL_Delay(time_to_wait);
    }

    cum_ticks = SDL_GetTicks();
}

void Game::render()
{
    SDL_SetRenderDrawColor(renderer, 21, 21, 21, 255);
    SDL_RenderClear(renderer);
    registry->get_system<RenderSystem>().update(renderer, asset_store, camera);
    if (debug)
    {
        registry->get_system<RenderColliderSystem>().update(renderer);
    }
    SDL_RenderPresent(renderer);
}
