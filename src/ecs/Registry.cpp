#include "ECS.hpp"
#include <string>
#include <memory>
using glm::vec2;

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

// ============================================================
// system management
// ============================================================
void Registry::add_entity_to_systems(Entity entity)
{
    const auto entity_id = entity.id();
    const auto &entity_component_signature = entity_component_signatures[entity_id];
    for (const auto &system_pair : systems)
    {
        // const auto& system_component_signature =
        const auto &system_component_signature = system_pair.second->get_component_signature();
        const auto should_add = (entity_component_signature & system_component_signature) == system_component_signature;

        if (should_add)
        {
            system_pair.second->add_entity(entity);
        }
    }
}

void Registry::update()
{
    for (const auto &entity : entities_to_add)
    {
        add_entity_to_systems(entity);
    }

    entities_to_add.clear();
}