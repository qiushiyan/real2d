#ifndef GAME_H
#define GAME_H

#include "ECS.hpp"
#include "Store.hpp"
#include "constants.hpp"
#include <SDL2/SDL.h>

class Game
{
private:
    bool running{false};
    std::uint64_t cum_ticks{0};
    SDL_Window *window;
    SDL_Renderer *renderer;
    std::shared_ptr<Registry> registry;
    std::shared_ptr<AssetStore> asset_store;
    std::shared_ptr<EventBus> event_bus;
    SDL_Rect camera;
    bool debug{false};
    bool show_gui{false};

public:
    Game();

    void init();
    void setup();
    void load_level(int level);
    void run();
    void destroy();
    void process_input();
    void render();
    void update();
};

#endif