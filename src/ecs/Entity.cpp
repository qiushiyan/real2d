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

void Entity::tag(const std::string &tag)
{
    registry->tag(*this, tag);
};
bool Entity::has_tag(const std::string &tag) const
{
    return registry->has_tag(*this, tag);
};

void Entity::group(const std::string &group)
{
    registry->group(*this, group);
};
bool Entity::belongs_to_group(const std::string &group) const
{
    return registry->belongs_to_group(*this, group);
};

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
