#include "Game.hpp"
#include "ECS.hpp"
#include <Logger.hpp>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <glm/glm.hpp>
#include <exception>
#include <stdexcept>
#include <memory>
#include <iostream>
using namespace std::string_literals;
using glm::vec2;

Game::Game()
{
    registry = std::make_unique<Registry>();
}

void Game::init()
{
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
    {
        Logger::error("SDL initialization failed."s);
        return;
    }

    SDL_DisplayMode displayMode;
    SDL_GetCurrentDisplayMode(0, &displayMode);

    window = SDL_CreateWindow(NULL, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, windowWidth, windowHeight, SDL_WINDOW_BORDERLESS);

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

    SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);

    running = true;
}

void Game::setup()
{
    auto entity = registry->create_entity();
    registry->add_component<TransformComponent>(entity, vec2(0.0, 0.0), vec2(0.0, 0.0), 0.0);
    registry->add_component<RigidBodyComponent>(entity, vec2(0.0, 0.0));
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

    auto current_ticks = SDL_GetTicks();
    auto time_to_wait = constants::TICKS_PER_FRAME - (current_ticks - cum_ticks);
    // delta time
    // float dt = (current_ticks - cum_ticks) / 1000.0f;

    if (time_to_wait > 0 && time_to_wait < constants::TICKS_PER_FRAME)
    {
        SDL_Delay(time_to_wait);
    }

    cum_ticks = SDL_GetTicks();
    registry->update();
    // MovementSystem::update(dt);
    // CollisionSystem::update(dt);
    // DamageSystem::update(dt);
}

void Game::render()
{
    SDL_SetRenderDrawColor(renderer, 21, 21, 21, 255);
    SDL_RenderClear(renderer);
    // auto surface = IMG_Load("../assets/images/tank-tiger-right.png");
    // auto texture = SDL_CreateTextureFromSurface(renderer, surface);
    // SDL_FreeSurface(surface);

    // auto dest_rect = SDL_Rect{
    //     static_cast<int>(playerPosition.x),
    //     static_cast<int>(playerPosition.y),
    //     32,
    //     32};
    // SDL_RenderCopy(renderer, texture, NULL, &dest_rect);
    // SDL_DestroyTexture(texture);
    SDL_RenderPresent(renderer);
}
