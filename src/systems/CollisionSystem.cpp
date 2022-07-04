#include "ECS.hpp"

CollisionSystem::CollisionSystem()
{
    require_component<TransformComponent>();
    require_component<BoxColliderComponent>();
}

bool CollisionSystem::collide(const Entity &entity, const Entity &other)
{
    auto &transform1 = entity.get_component<TransformComponent>();
    auto &transform2 = other.get_component<TransformComponent>();
    auto &box1 = entity.get_component<BoxColliderComponent>();
    auto &box2 = other.get_component<BoxColliderComponent>();

    bool B_left_A = transform1.position.x - box1.offset.x < transform2.position.x + box2.width + box2.offset.x;
    bool A_left_B = transform1.position.x + box1.width + box1.offset.x > transform2.position.x - box2.offset.x;
    bool B_top_A = transform1.position.y - box1.offset.y < transform2.position.y + box2.height + box2.offset.y;
    bool A_top_B = transform1.position.y + box1.height + box1.offset.y > transform2.position.y - box2.offset.y;

    if (B_left_A && A_left_B && B_top_A && A_top_B)
    {
        return true;
    }
    else
    {
        return false;
    }
};

void CollisionSystem::update(std::shared_ptr<EventBus> event_bus)
{
    const auto _entities = entities();
    for (auto it = _entities.begin(); it != _entities.end(); it++)
    {
        for (auto it_other = it + 1; it_other != _entities.end(); it_other++)
        {
            if (collide(*it, *it_other))
            {
                event_bus->emit<CollisionEvent>(*it, *it_other);
            }
        }
    }
}