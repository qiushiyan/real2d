#ifndef GAME_H
#define GAME_H

#include "ECS.hpp"
#include "constants.hpp"
#include <SDL2/SDL.h>

class Game
{
private:
    bool running{false};
    std::uint64_t cum_ticks{0};
    SDL_Window *window;
    SDL_Renderer *renderer;
    std::unique_ptr<Registry> registry;

public:
    Game();

    void init();
    void setup();
    void run();
    void destroy();
    void process_input();
    void render();
    void update();

    const int windowHeight{constants::windowHeight};
    const int windowWidth{constants::windowWidth};
};

#endif