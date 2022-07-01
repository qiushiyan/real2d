#ifndef STORE_H
#define STORE_H

#include "SDL2/SDL.h"
#include <map>
#include <string>

class AssetStore
{
private:
    std::map<std::string, SDL_Texture *> textures;

public:
    AssetStore() = default;
    ~AssetStore();
    void clear_assets();
    void add_texture(SDL_Renderer *renderer, const std::string &name, const std::string &file_path);
    SDL_Texture *get_texture(const std::string &name);
};

#endif