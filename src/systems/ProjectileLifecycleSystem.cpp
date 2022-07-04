#include "ECS.hpp"

ProjectileLifecycleSystem::ProjectileLifecycleSystem()
{
    require_component<ProjectileComponent>();
};

void ProjectileLifecycleSystem::update()
{
    for (auto entity : entities())
    {
        const auto &projectile = entity.get_component<ProjectileComponent>();
        const auto &transform = entity.get_component<TransformComponent>();
        if (SDL_GetTicks() > (projectile.start_time + projectile.duration))
        {
            entity.kill();
        }

        if (transform.position.x < 0 || transform.position.x > constants::map_width || transform.position.y < 0 || transform.position.y > constants::map_height)
        {
            entity.kill();
        }
    }
}