#include "ECS.hpp"
#include <iostream>
#include <Remap.hpp>

RenderHealthSystem::RenderHealthSystem()
{
    require_component<HealthComponent>();
    require_component<TransformComponent>();
    require_component<SpriteComponent>();
}

void RenderHealthSystem::update(SDL_Renderer *renderer, std::shared_ptr<AssetStore> asset_store, SDL_Rect &camera)
{
    for (const auto &entity : entities())
    {
        const auto &health = entity.get_component<HealthComponent>();
        const auto &transform = entity.get_component<TransformComponent>();
        const auto &sprite = entity.get_component<SpriteComponent>();

        SDL_Color healthbar_color = {255, 255, 255};

        if (health.health >= 0 && health.health < 60)
        {
            float color = Remap(0, 60, 50, 255, health.health);
            healthbar_color = {static_cast<u_int8_t>(color), 0, 0};
        }
        else if (health.health >= 60 && health.health < 80)
        {
            healthbar_color = {255, 255, 0};
        }
        else if (health.health >= 80 && health.health <= 100)
        {
            healthbar_color = {0, 255, 0};
        }

        // position healthbar to top right of the entity
        vec2 healthbar_position = {
            (transform.position.x + sprite.width / 2 + transform.scale.x) - camera.x,
            transform.position.y - camera.y};

        SDL_Rect healthbar_rect = {
            (int)healthbar_position.x,
            (int)healthbar_position.y,
            (int)(constants::healthbar_width * health.health / 100),
            (int)constants::healthbar_height};

        SDL_Rect healthbar_outline = {
            (int)healthbar_position.x,
            (int)healthbar_position.y,
            (int)constants::healthbar_width,
            (int)constants::healthbar_height};

        // create health text
        std::string health_text = std::to_string(health.health);
        SDL_Surface *surface = TTF_RenderText_Blended(asset_store->get_font("sub-font"), health_text.c_str(), healthbar_color);
        SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_FreeSurface(surface);

        // get text width and height
        int text_width, text_height;
        SDL_QueryTexture(texture, NULL, NULL, &text_width, &text_height);
        SDL_Rect text_rect = {
            (int)healthbar_position.x + constants::healthbar_width + 1,
            (int)healthbar_position.y + constants::healthbar_height / 2,
            text_width,
            text_height};

        // draw healthbar and text

        SDL_SetRenderDrawColor(renderer, healthbar_color.r, healthbar_color.g, healthbar_color.b, 255);
        SDL_RenderFillRect(renderer, &healthbar_rect);
        SDL_RenderCopy(renderer, texture, NULL, &text_rect);
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderDrawRect(renderer, &healthbar_outline);

        // release texture resource
        SDL_DestroyTexture(texture);
    }
}