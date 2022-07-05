#include "ECS.hpp"

ProjectileEmitSystem::ProjectileEmitSystem()
{
    require_component<ProjectileEmitterComponent>();
    require_component<TransformComponent>();
}

void ProjectileEmitSystem::emit_from(const Entity &entity, bool const_direction)
{

    auto &projectile = entity.get_component<ProjectileEmitterComponent>();
    const auto &transform = entity.get_component<TransformComponent>();
    const auto &rigid_body = entity.get_component<RigidBodyComponent>();

    auto projectile_position = transform.position;
    if (entity.has_component<SpriteComponent>())
    {
        auto sprite = entity.get_component<SpriteComponent>();
        projectile_position += glm::vec2(sprite.width / 2, sprite.height / 2);
    }

    auto projectile_velocity = projectile.velocity;
    if (!const_direction)
    {
        int direction_x = rigid_body.velocity.x > 0 ? 1 : (rigid_body.velocity.x < 0 ? -1 : 0);
        int direction_y = rigid_body.velocity.y > 0 ? 1 : (rigid_body.velocity.y < 0 ? -1 : 0);
        ;
        projectile_velocity.x *= direction_x;
        projectile_velocity.y *= direction_y;
    }

    auto p = entity.registry->create_entity();
    p.group("projectiles");
    p.add_component<TransformComponent>(projectile_position, transform.scale, transform.rotation);
    p.add_component<RigidBodyComponent>(projectile_velocity);
    p.add_component<SpriteComponent>("bullet-image", 4, 4, 1);
    p.add_component<BoxColliderComponent>(4, 4);
    p.add_component<ProjectileComponent>(projectile.duration, projectile.is_friendly, projectile.damage);

    projectile.last_emission_time = SDL_GetTicks();
}

void ProjectileEmitSystem::on_mouse_clicked(MouseClickedEvent &event)
{
    switch (event.button)
    {
    case (SDL_BUTTON_LEFT):
        for (const auto &entity : entities())
        {
            emit_from(entity, false);
        }
        break;
    }
}

void ProjectileEmitSystem::on_key_pressed(KeyPressedEvent &e)
{
    switch (e.key)
    {
    case SDLK_SPACE:
        for (const auto &entity : entities())
        {
            emit_from(entity, false);
        }
        break;
    }
}

void ProjectileEmitSystem::subscribe_events(std::shared_ptr<EventBus> event_bus)
{
    event_bus->subscribe(this, &ProjectileEmitSystem::on_key_pressed);
    event_bus->subscribe(this, &ProjectileEmitSystem::on_mouse_clicked);
}

void ProjectileEmitSystem::update(std::shared_ptr<Registry> registry)
{
    for (const auto &entity : entities())
    {
        const auto &projectile = entity.get_component<ProjectileEmitterComponent>();
        if (projectile.freq != 0 && SDL_GetTicks() - projectile.last_emission_time > projectile.freq)
        {
            emit_from(entity, true);
        }
    }
}