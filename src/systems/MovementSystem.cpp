#include "ECS.hpp"

MovementSystem::MovementSystem()
{

    require_component<TransformComponent>();
    require_component<RigidBodyComponent>();
}

void MovementSystem::update(const float dt)
{
    for (const auto &entity : entities())
    {
        auto &transform = entity.get_component<TransformComponent>();
        auto &rigid_body = entity.get_component<RigidBodyComponent>();

        transform.position.x += rigid_body.velocity.x * dt;
        transform.position.y += rigid_body.velocity.y * dt;
        // Logger::info("Entity " + std::to_string(entity.id()) + " is now " + std::to_string(transform.position.x) + "," + std::to_string(transform.position.y));
    }
}