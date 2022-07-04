#include "ECS.hpp"

CameraMovementSystem::CameraMovementSystem()
{
    require_component<CameraFollowComponent>();
    require_component<TransformComponent>();
}

void CameraMovementSystem::update(SDL_Rect &camera)
{
    for (const auto &entity : entities())
    {
        const auto transform = entity.get_component<TransformComponent>();
        if (transform.position.x + camera.w / 2 < constants::map_width)
        {
            camera.x = transform.position.x - constants::window_width / 2;
        }
        if (transform.position.y + camera.h / 2 < constants::window_height)
        {
            camera.y = transform.position.y - constants::window_height / 2;
        }

        camera.x = camera.x < 0 ? 0 : camera.x;
        camera.y = camera.y < 0 ? 0 : camera.y;
        camera.x = camera.x > camera.w ? camera.w : camera.x;
        camera.y = camera.y > camera.h ? camera.h : camera.y;
    }
}