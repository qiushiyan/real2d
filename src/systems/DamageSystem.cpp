#include "ECS.hpp"

DamageSystem::DamageSystem()
{
    require_component<BoxColliderComponent>();
    require_component<HealthComponent>();
}

void DamageSystem::on_projectile_hit_player(Entity &projectile, Entity &player)
{
    const auto &projectile_component = projectile.get_component<ProjectileComponent>();
    if (!projectile_component.is_friendly && player.has_component<HealthComponent>())
    {
        auto &health_component = player.get_component<HealthComponent>();
        health_component.health -= projectile_component.damage;
        if (health_component.health <= 0)
        {
            player.kill();
        }
        projectile.kill();
    }
}

void DamageSystem::on_projectile_hit_enemy(Entity &projectile, Entity &enemy)
{
    const auto &projectile_component = projectile.get_component<ProjectileComponent>();
    if (projectile_component.is_friendly && enemy.has_component<HealthComponent>())
    {
        auto &health_component = enemy.get_component<HealthComponent>();
        health_component.health -= projectile_component.damage;
        if (health_component.health <= 0)
        {
            enemy.kill();
        }
        projectile.kill();
    }
}

void DamageSystem::on_collision(CollisionEvent &e)
{
    Entity a = e.a;
    Entity b = e.b;

    if (a.has_tag("player") && b.belongs_to_group("projectiles"))
    {
        on_projectile_hit_player(b, a);
    }

    if (a.belongs_to_group("projectiles") && b.has_tag("player"))
    {
        on_projectile_hit_player(a, b);
    }

    if (a.belongs_to_group("enemies") && b.belongs_to_group("projectiles"))
    {
        on_projectile_hit_enemy(b, a);
    }

    if (a.belongs_to_group("projectiles") && b.belongs_to_group("enemies"))
    {
        on_projectile_hit_enemy(a, b);
    }
}

void DamageSystem::subscribe_events(std::shared_ptr<EventBus> event_bus)
{
    event_bus->subscribe(this, &DamageSystem::on_collision);
}

void DamageSystem::update()
{
}