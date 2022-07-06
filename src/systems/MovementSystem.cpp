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

        transform.position.x += rigid_body.velocity.x * dt;
        transform.position.y += rigid_body.velocity.y * dt;

        // kill entity outside the map
        bool outside_map = transform.position.x < 0 || transform.position.x > constants::map_width || transform.position.y < 0 || transform.position.y > constants::map_height;
        if (outside_map && !entity.has_tag("player"))
        {
            entity.kill();
            // Logger::info("Entity " + std::to_string(entity.id()) + " is now " + std::to_string(transform.position.x) + "," + std::to_string(transform.position.y));
        }
    }