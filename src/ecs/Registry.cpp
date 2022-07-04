#include "ECS.hpp"
using glm::vec2;

Entity Registry::create_entity()
{
    int id;
    if (free_ids.empty())
    {
        id = num_entities++;
        if (id > entity_component_signatures.size())
        {
            entity_component_signatures.resize(id + 1);
        }
    }
    else
    {
        id = free_ids.front();
        free_ids.pop_front();
    }

    auto entity{Entity{id, this}};
    entities_to_add.insert(entity);

    return entity;
}

void Registry::kill_entity(Entity entity)
{
    entities_to_kill.insert(entity);
}

// ============================================================
// add and remove entities to matching systems
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

void Registry::remove_entity_from_systems(Entity entity)
{
    for (auto &system_pair : systems)
    {
        system_pair.second->remove_entity(entity);
    }
}

void Registry::update()
{
    for (const auto &entity : entities_to_add)
    {
        add_entity_to_systems(entity);
    }
    entities_to_add.clear();

    for (auto &entity : entities_to_kill)
    {
        remove_entity_from_systems(entity);

        const auto entity_id = entity.id();
        entity_component_signatures[entity_id].reset();
        free_ids.push_back(entity_id);
    }

    entities_to_kill.clear();
}