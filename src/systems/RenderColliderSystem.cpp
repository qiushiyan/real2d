#include "ECS.hpp"

RenderColliderSystem::RenderColliderSystem()
{
    require_component<TransformComponent>();
    require_component<BoxColliderComponent>();
}

void RenderColliderSystem::update(SDL_Renderer *renderer)
{
    for (const auto &entity : entities())
    {
        const auto &transform = entity.get_component<TransformComponent>();
        const auto &box = entity.get_component<BoxColliderComponent>();

        SDL_Rect collider_rect = {
            (int)(transform.position.x - box.offset.x),
            (int)(transform.position.y - box.offset.y),
            (int)(box.width + 2 * box.offset.x),
            (int)(box.height + 2 * box.offset.y)};
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        SDL_RenderDrawRect(renderer, &collider_rect);
    }
}