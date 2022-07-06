#include "ECS.hpp"

MovementSystem::MovementSystem()
{

    require_component<TransformComponent>();
    require_component<RigidBodyComponent>();
}

void MovementSystem::update(float dt)
{
    for (const auto &entity : entities())
    {
        auto &transform = entity.get_component<TransformComponent>();
        auto &rigid_body = entity.get_component<RigidBodyComponent>();

        vec2 movement = rigid_body.velocity * dt;
        if (entity.has_component<SprintComponent>())
        {
            auto &sprint = entity.get_component<SprintComponent>();
            bool no_sprint = (SDL_GetTicks() - sprint.last_sprint_time > sprint.sprint_duration);
            movement *= (sprint.in_sprint ? sprint.sprint_speed : 1);
            if (sprint.in_sprint && no_sprint)
            {
                sprint.in_sprint = false;
            }
        }

        transform.position += movement;

        // kill entity outside the map
        if (!entity.has_tag("player"))
        {
            bool outside_map = transform.position.x < 0 || transform.position.x > constants::map_width || transform.position.y < 0 || transform.position.y > constants::map_height;
            if (outside_map)
            {
                entity.kill();
            }
        }

        if (entity.has_tag("player"))
        {
            auto &sprite = entity.get_component<SpriteComponent>();
            if (transform.position.x < 0)
            {
                transform.position.x = 0;
            }

            if (transform.position.x > constants::map_width - sprite.width)
            {
                transform.position.x = constants::map_width - sprite.width;
            }

            if (transform.position.y < 0)
            {
                transform.position.y = 0;
            }

            if (transform.position.y > constants::map_height - sprite.height)
            {
                transform.position.y = constants::map_height - sprite.height;
            }
        }
    }
}