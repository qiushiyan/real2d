#include "ECS.hpp"
Entity Registry::create_entity()
{
    auto id = num_entities++;
    if (id > entity_component_signatures.size())
    {
        entity_component_signatures.resize(id + 1);
    }
    auto entity{Entity{id}};
    entities_to_add.insert(entity);
    return entity;
}

void Registry::update()
{
}