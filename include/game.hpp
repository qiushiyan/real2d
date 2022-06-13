#ifndef GAME_H
#define GAME_H

#include <SDL2/SDL.h>
#include "constants.hpp"

class Game
{
private:
    bool running{false};
    SDL_Window *window;
    SDL_Renderer *renderer;

public:
    Game(){};
    ~Game(){};

    void init();
    void run();
    void destroy();
    void process_input();
    void render();
    void update();

    const int windowHeight{constants::windowHeight};
    const int windowWidth{constants::windowWidth};
};

#endif