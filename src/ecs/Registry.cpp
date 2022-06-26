#include "ECS.hpp"
#include "Logger.hpp"
#include <string>
#include <memory>
#include "iostream"

Entity Registry::create_entity()
{
    auto id = num_entities++;
    if (id > entity_component_signatures.size())
    {
        entity_component_signatures.resize(id + 1);
    }
    auto entity{Entity{id}};
    Logger::info("Created entity with id: " + std::to_string(id));
    entities_to_add.insert(entity);

    return entity;
}

// ============================================================
// component management
// ============================================================
template <typename TComponent, typename... TArgs>
void Registry::add_component(Entity entity, TArgs &&...args)
{
    const auto entity_id = entity.id();
    const auto component_id = Component<TComponent>::id();

    if (component_id > component_pools.size())
    {
        component_pools.resize(component_id + 1, nullptr);
    }

    if (!component_pools[component_id])
    {
        component_pools[component_id] = std::make_shared<Pool<TComponent>>();
    }

    std::shared_ptr<Pool<TComponent>> component_pool_ptr = component_pools[component_id];

    if (entity_id > component_pool_ptr->size())
    {
        component_pool_ptr->resize(num_entities);
    }

    // create new component
    TComponent new_component(std::forward<TArgs>(args)...);
    // add component to the pool, use entity id as index
    component_pool_ptr->set(entity_id, new_component);
    // set entity signature
    entity_component_signatures[entity_id].set(component_id);
}

template <typename TComponent>
void Registry::remove_component(Entity entity)
{
    const auto entity_id = entity.id();
    const auto component_id = Component<TComponent>::id();
    entity_component_signatures[entity_id].set(component_id, false);
}

template <typename TComponent>
bool Registry::has_component(Entity entity) const
{
    const auto entity_id = entity.id();
    const auto component_id = Component<TComponent>::id();
    return entity_component_signatures[entity_id].test(component_id);
}

// ============================================================
// system management
// ============================================================
template <typename TSystem, typename... TArgs>
void Registry::add_system(TArgs &&...args)
{
    std::shared_ptr<TSystem> new_system_ptr = std::make_shared<TSystem>(std::forward<TArgs>(args)...);
    systems.insert(std::make_pair(std::type_index(typeid(TSystem)), new_system_ptr));
}

template <typename TSystem>
void Registry::remove_system()
{
    auto it = systems.find(std::type_index(typeid(TSystem)));
    systems.erase(it);
};

template <typename TSystem>
bool Registry::has_system() const
{
    return systems.find(std::type_index(typeid(TSystem))) != systems.end() ? true : false;
};
template <typename TSystem>
TSystem &Registry::get_system() const
{
    auto it = systems.find(std::type_index(typeid(TSystem)));
    return *(std::static_pointer_cast<TSystem>(it->second));
};

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