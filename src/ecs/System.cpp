#include "ECS.hpp"
#include <vector>
#include <algorithm>

void System::add_entity(Entity entity)
{
    // if has sprite component, insert into vector sorted by z_index
    bool has_sprite = entity.has_component<SpriteComponent>();
    if (has_sprite)
    {
        auto it = std::upper_bound(_entities.begin(), _entities.end(), entity, [](const Entity &entity, const Entity &other)
                                   {
            if (!other.has_component<SpriteComponent>()) {
                return false;
            } else {
                return entity.get_component<SpriteComponent>().z_index < other.get_component<SpriteComponent>().z_index;
            } });

        _entities.insert(it, entity);
    }
    else
    {
        _entities.push_back(entity);
    }
};
void System::remove_entity(Entity entity)
{
    auto it = std::remove_if(_entities.begin(), _entities.end(),
                             [entity](const Entity &e)
                             { return e == entity; });

    _entities.erase(it, _entities.end());
}
std::vector<Entity> System::entities() const
{
    return _entities;
};
const Signature &System::get_component_signature() const
{
    return component_signature;
};
