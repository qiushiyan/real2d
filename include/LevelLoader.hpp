#include <SDL2/SDL.h>
#include <sol/sol.hpp>
#include "ECS.hpp"
#include "Store.hpp"

class LevelLoader
{
private:
    std::shared_ptr<Registry> registry;
    std::shared_ptr<AssetStore> asset_store;
    SDL_Renderer *renderer;

public:
    LevelLoader(std::shared_ptr<Registry> registry, std::shared_ptr<AssetStore> asset_store, SDL_Renderer *renderer);
    ~LevelLoader() = default;

    void load(sol::state &lua, int level);
};