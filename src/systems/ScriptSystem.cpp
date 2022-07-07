#include "ECS.hpp"

ScriptSystem::ScriptSystem()
{
    require_component<ScriptComponent>();
}

void set_position(Entity entity, int x, int y)
{
    if (entity.has_component<TransformComponent>())
    {
        auto &transform = entity.get_component<TransformComponent>();
        transform.position.x = x;
        transform.position.y = y;
    }
}

std::tuple<int, int> get_position(Entity &entity)
{
    if (entity.has_component<TransformComponent>())
    {
        const auto &transform = entity.get_component<TransformComponent>();
        return std::make_tuple(transform.position.x, transform.position.y);
    }
    else
    {
        return std::make_tuple(0.0, 0.0);
    }
}

std::tuple<double, double> get_velocity(Entity entity)
{
    if (entity.has_component<RigidBodyComponent>())
    {
        const auto rigid_body = entity.get_component<RigidBodyComponent>();
        return std::make_tuple(rigid_body.velocity.x, rigid_body.velocity.y);
    }
    else
    {
        return std::make_tuple(0.0, 0.0);
    }
}

void set_velocity(Entity entity, double x, double y)
{
    if (entity.has_component<RigidBodyComponent>())
    {
        auto &rigid_body = entity.get_component<RigidBodyComponent>();
        rigid_body.velocity.x = x;
        rigid_body.velocity.y = y;
    }
}

void set_rotation(Entity entity, double angle)
{
    if (entity.has_component<TransformComponent>())
    {
        auto &transform = entity.get_component<TransformComponent>();
        transform.rotation = angle;
    }
}

void set_animation_frame(Entity entity, int frame)
{
    if (entity.has_component<AnimationComponent>())
    {
        auto &animation = entity.get_component<AnimationComponent>();
        animation.current_frame = frame;
    }
}

void set_projectile_velocity(Entity entity, double x, double y)
{
    if (entity.has_component<ProjectileEmitterComponent>())
    {
        auto &projectile = entity.get_component<ProjectileEmitterComponent>();
        projectile.velocity.x = x;
        projectile.velocity.y = y;
    }
}

void ScriptSystem::create_lua_bindings(sol::state &lua)
{
    lua.new_usertype<Entity>(
        "entity",
        "id", &Entity::id,
        "kill", &Entity::kill,
        "has_tag", &Entity::has_tag,
        "belongs_to_group", &Entity::belongs_to_group);
    lua.set_function("set_position", set_position);
    lua.set_function("get_position", get_position);
    lua.set_function("set_velocity", set_velocity);
    lua.set_function("get_velocity", get_velocity);
    lua.set_function("set_rotation", set_rotation);
    lua.set_function("set_animation_frame", set_animation_frame);
    lua.set_function("set_projectile_velocity", set_projectile_velocity);
}

void ScriptSystem::update(double delta_time, int elapsed_time)
{
    for (auto entity : entities())
    {
        auto &script = entity.get_component<ScriptComponent>();
        script.fun(entity, delta_time, elapsed_time);
    }
}