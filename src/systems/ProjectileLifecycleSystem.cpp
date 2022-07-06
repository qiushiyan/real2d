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
        if (SDL_GetTicks() > (projectile.start_time + projectile.duration))
        {
            entity.kill();
        }
    }
}