#include "ECS.hpp"

void Entity::id(const int new_id)
{
    _id = new_id;
}

int Entity::id() const
{
    return _id;
}

void Entity::kill() const
{
    registry->kill_entity(*this);
}

bool Entity::operator==(const Entity &other) const
{
    return _id == other.id();
}
bool Entity::operator!=(const Entity &other) const
{
    return _id != other.id();
}
bool Entity::operator>(const Entity &other) const
{
    return _id > other.id();
}
bool Entity::operator<(const Entity &other) const
{
    return _id < other.id();
}
