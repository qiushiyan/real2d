#include "ECS.hpp"
using glm::vec2;

// ============================================================
// create and kill entity
// ============================================================
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
// add and remove tags and groups
// ============================================================
void Registry::tag(Entity entity, const std::string &tag)
{

    tag_per_entity.emplace(entity.id(), tag);
    entity_per_tag.emplace(tag, entity);
}

bool Registry::has_tag(Entity entity, const std::string &tag) const
{
    if (tag_per_entity.find(entity.id()) == tag_per_entity.end())
    {
        return false;
    }

    if (entity_per_tag.find(tag) == entity_per_tag.end())
    {
        return false;
    }

    return entity_per_tag.find(tag)->second == entity;
}

Entity Registry::get_entity_by_tag(const std::string &tag) const
{
    return entity_per_tag.at(tag);
}

void Registry::remove_entity_tag(Entity entity)
{
    auto it = tag_per_entity.find(entity.id());
    if (it != tag_per_entity.end())
    {
        auto tag = it->second;
        entity_per_tag.erase(tag);
        tag_per_entity.erase(it);
    }
}

void Registry::group(Entity entity, const std::string &group)
{
    auto it = entities_per_group.find(group);
    if (it == entities_per_group.end())
    {
        entities_per_group.emplace(group, std::set<Entity>{});
    }

    entities_per_group[group].emplace(entity);
    group_per_entity.emplace(entity.id(), group);
}

bool Registry::belongs_to_group(Entity entity, const std::string &group) const
{
    if (entities_per_group.find(group) == entities_per_group.end())
    {
        return false;
    }

    auto entities = entities_per_group.at(group);
    return entities.find(entity) != entities.end();
}

std::vector<Entity> Registry::get_entities_by_group(const std::string &group) const
{
    auto &entities = entities_per_group.at(group);
    return std::vector<Entity>(entities.begin(), entities.end());
}

void Registry::remove_entity_group(Entity entity)
{
    auto it = group_per_entity.find(entity.id());
    if (it != group_per_entity.end())
    {
        auto group = entities_per_group.find(it->second);
        if (group != entities_per_group.end())
        {
            auto it = group->second.find(entity);
            group->second.erase(it);
        }
        group_per_entity.erase(it);
    }
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
        remove_entity_tag(entity);
        remove_entity_group(entity);
    }

    entities_to_kill.clear();
}