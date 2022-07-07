#include "ECS.hpp"

MovementSystem::MovementSystem()
{
    require_component<TransformComponent>();
    require_component<RigidBodyComponent>();
}

void on_enemy_hits_obstacles(Entity &a, Entity &b)
{
    if (a.has_component<RigidBodyComponent>() && a.has_component<SpriteComponent>())
    {
        auto &rigid_body = a.get_component<RigidBodyComponent>();
        auto &sprite = a.get_component<SpriteComponent>();
        if (rigid_body.velocity.x != 0)
        {
            rigid_body.velocity.x *= -1;
            sprite.flip = sprite.flip == SDL_FLIP_NONE ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;
        }
        if (rigid_body.velocity.y != 0)
        {
            rigid_body.velocity.y *= -1;
            sprite.flip = sprite.flip == SDL_FLIP_NONE ? SDL_FLIP_VERTICAL : SDL_FLIP_NONE;
        }
    }
}

void MovementSystem::on_collision(CollisionEvent &e)
{
    Entity a = e.a;
    Entity b = e.b;

    if (a.belongs_to_group("enemies") && b.belongs_to_group("obstacles"))
    {
        on_enemy_hits_obstacles(a, b);
    }
    if (a.belongs_to_group("obstacles") && b.belongs_to_group("enemies"))
    {
        on_enemy_hits_obstacles(b, a);
    }
}

void MovementSystem::subscribe_events(std::shared_ptr<EventBus> event_bus)
{
    event_bus->subscribe(this, &MovementSystem::on_collision);
};

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
            int padding_left = 10;
            int padding_top = 10;
            int padding_right = 140;
            int padding_bottom = 140;
            auto &sprite = entity.get_component<SpriteComponent>();
            if (transform.position.x < padding_left)
            {
                transform.position.x = padding_left;
            }

            if (transform.position.x > constants::map_width - sprite.width - padding_right)
            {
                transform.position.x = constants::map_width - sprite.width - padding_right;
            }

            if (transform.position.y < padding_top)
            {
                transform.position.y = padding_top;
            }

            if (transform.position.y > constants::map_height - sprite.height - padding_bottom)
            {
                transform.position.y = constants::map_height - sprite.height - padding_bottom;
            }
        }
    }
}