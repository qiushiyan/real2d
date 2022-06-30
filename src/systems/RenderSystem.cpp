#include "ECS.hpp"
#include <SDL2/SDL.h>

RenderSystem::RenderSystem()
{
    require_component<TransformComponent>();
    require_component<SpriteComponent>();
}

void RenderSystem::update(SDL_Renderer *renderer)
{
    for (const auto &entity : entities())
    {
        auto &transform = entity.get_component<TransformComponent>();
        auto &sprite = entity.get_component<SpriteComponent>();

        SDL_Rect obj = {(int)transform.position.x,
                        (int)transform.position.y,
                        sprite.width,
                        sprite.height};
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderFillRect(renderer, &obj);
    }
};