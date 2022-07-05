#include "ECS.hpp"

RenderTextSystem::RenderTextSystem()
{
    require_component<TextComponent>();
}

void RenderTextSystem::update(SDL_Renderer *renderer, std::shared_ptr<AssetStore> asset_store, const SDL_Rect &camera)
{
    for (const auto &entity : entities())
    {
        const auto text_component = entity.get_component<TextComponent>();
        const auto font = asset_store->get_font(text_component.font_name);
        const auto label = text_component.text.c_str();
        SDL_Surface *surface = TTF_RenderText_Blended(font, label, text_component.color);

        SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);

        SDL_FreeSurface(surface);

        // find width and height of the font
        int text_width = 0;
        int text_height = 0;

        SDL_QueryTexture(texture, NULL, NULL, &text_width, &text_height);

        SDL_Rect dest_rect = {
            (int)(text_component.position.x - (text_component.is_fixed ? 0 : camera.x)),
            (int)(text_component.position.y - (text_component.is_fixed ? 0 : camera.y)),
            (int)(text_width),
            (int)(text_height)};

        SDL_RenderCopy(renderer, texture, NULL, &dest_rect);
    }
}