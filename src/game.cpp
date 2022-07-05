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
    registry = std::make_shared<Registry>();
    asset_store = std::make_shared<AssetStore>();
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

    if (TTF_Init() != 0)
    {
        Logger::error("TTF initialization failed."s);
        return;
    }

    TTF_Init();

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
    registry->add_system<ProjectileEmitSystem>();
    registry->add_system<ProjectileLifecycleSystem>();
    registry->add_system<RenderTextSystem>();

    registry->get_system<DamageSystem>().subscribe_events(event_bus);
    registry->get_system<KeyboardControlSystem>().subscribe_events(event_bus);
    registry->get_system<ProjectileEmitSystem>().subscribe_events(event_bus);
    // add textures
    asset_store->add_texture(renderer, "radar-image", "../assets/images/radar.png");
    asset_store->add_texture(renderer, "tilemap-image", "../assets/tilemaps/jungle.png");
    asset_store->add_texture(renderer, "chopper-image", "../assets/images/chopper-spritesheet.png");
    asset_store->add_texture(renderer, "tank-image-left", "../assets/images/tank-tiger-left.png");
    asset_store->add_texture(renderer, "tank-image-right", "../assets/images/tank-tiger-right.png");
    asset_store->add_texture(renderer, "bullet-image", "../assets/images/bullet.png");
    asset_store->add_font(renderer, "main", "../assets/fonts/whatnot.ttf", 20);

    // test animation
    auto chopper = registry->create_entity();
    chopper.tag("player");
    chopper.add_component<TransformComponent>(vec2(10, 20), vec2(1, 1));
    chopper.add_component<RigidBodyComponent>(vec2(100, 0));
    chopper.add_component<SpriteComponent>("chopper-image", tile_size, tile_size, 2, false, 0, tile_size);
    chopper.add_component<AnimationComponent>(2, 12, true);
    chopper.add_component<BoxColliderComponent>(tile_size, tile_size);
    chopper.add_component<KeyboardControlComponent>(vec2(0, -100), vec2(100, 0), vec2(0, 100), vec2(-100, 0));
    chopper.add_component<CameraFollowComponent>();
    chopper.add_component<HealthComponent>();
    chopper.add_component<ProjectileEmitterComponent>(vec2(150, 150), 0, 5000, true, 10);

    // test fixed entity
    auto radar = registry->create_entity();
    radar.tag("radar");
    radar.add_component<TransformComponent>(vec2(window_width - tile_size, tile_size / 4), vec2(1, 1));
    radar.add_component<SpriteComponent>("radar-image", 64, 64, 1, true);
    radar.add_component<AnimationComponent>(8, 4, true);

    // test collision
    auto tank1 = registry->create_entity();
    tank1.group("enemies");
    tank1.add_component<TransformComponent>(vec2(50, 100), vec2(1, 1), 0.0);
    tank1.add_component<RigidBodyComponent>(vec2(0, 0));
    tank1.add_component<SpriteComponent>("tank-image-right", tile_size, tile_size, 3);
    tank1.add_component<BoxColliderComponent>(tile_size, tile_size);
    tank1.add_component<ProjectileEmitterComponent>(vec2(200, 0), 1000, 5000, false, 10);
    tank1.add_component<HealthComponent>(30);

    // test text
    Entity label = registry->create_entity();
    SDL_Color color = {0, 255, 0};
    label.add_component<TextComponent>(vec2(window_width / 2 - 40, 10), "Chopper 1.0", "main", color);

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
            tile.group("tiles");
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
    registry->get_system<ProjectileEmitSystem>().update(registry);
    registry->get_system<ProjectileLifecycleSystem>().update();

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
    registry->get_system<RenderTextSystem>().update(renderer, asset_store, camera);
    if (debug)
    {
        registry->get_system<RenderColliderSystem>().update(renderer, camera);
    }
    SDL_RenderPresent(renderer);
}
