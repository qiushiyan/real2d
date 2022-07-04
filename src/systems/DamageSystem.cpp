#include "ECS.hpp"

DamageSystem::DamageSystem()
{
    require_component<BoxColliderComponent>();
}

void DamageSystem::on_collision(CollisionEvent &e)
{
    e.a.kill();
    e.b.kill();
}

void DamageSystem::subscribe_events(std::shared_ptr<EventBus> event_bus)
{
    event_bus->subscribe(this, &DamageSystem::on_collision);
}

void DamageSystem::update()
{
}