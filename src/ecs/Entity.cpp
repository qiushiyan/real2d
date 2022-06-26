#include "ECS.hpp"

void Entity::id(const std::uint8_t new_id)
{
    _id = new_id;
}

std::uint8_t Entity::id() const
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
