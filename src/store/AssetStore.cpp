#include "Store.hpp"
#include <SDL2/SDL_image.h>

AssetStore::~AssetStore()
{
    clear_assets();
}

void AssetStore::clear_assets()
{
    for (const auto &texture : textures)
    {
        SDL_DestroyTexture(texture.second);
    }

    for (const auto &font : fonts)
    {
        TTF_CloseFont(font.second);
    }

    textures.clear();
    fonts.clear();
}

void AssetStore::add_texture(SDL_Renderer *renderer, const std::string &name, const std::string &file_path)
{
    const auto surface = IMG_Load(file_path.c_str());
    const auto texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    textures.emplace(name, texture);
}

SDL_Texture *AssetStore::get_texture(const std::string &name)
{
    return textures[name];
}

void AssetStore::add_font(SDL_Renderer *renderer, const std::string &name, const std::string &file_path, int size)
{
    const auto font = TTF_OpenFont(file_path.c_str(), size);
    fonts.emplace(name, font);
}

TTF_Font *AssetStore::get_font(const std::string &name)
{
    return fonts[name];
}