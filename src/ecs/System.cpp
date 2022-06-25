#include "ECS.hpp"
#include <vector>
#include <algorithm>
void System::add_entity(Entity entity)
{
    _entities.push_back(entity);
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
