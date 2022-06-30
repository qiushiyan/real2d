#include "ECS.hpp"

void Entity::id(const int new_id)
{
    _id = new_id;
}

int Entity::id() const
{
    return _id;
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
