#ifndef ECS_H
#define ECS_H
#include "constants.hpp"
#include <vector>
#include <unordered_map>
#include <typeindex>
#include <set>
#include <glm/glm.hpp>
#include <memory>
#include <iostream>
#include <Logger.hpp>
#include <SDL2/SDL.h>
#include "Store.hpp"

using constants::Signature;
using glm::vec2;
using namespace std::string_literals;

// ============================================================
// Pool
// ============================================================

class IPool
{
};

template <typename T>
class Pool : public IPool
{
private:
    std::vector<T> data;

public:
    Pool(int n = 1000)
    {
        data.resize(n);
    };
    ~Pool()
    {
        data.clear();
    };

    bool empty() const
    {
        return data.empty();
    };
    std::size_t size() const
    {
        return data.size();
    };
    void clear()
    {
        data.clear();
    };
    void resize(int n)
    {
        data.resize(n);
    };
    void add(T element)
    {
        data.push_back(element);
    };
    void set(int index, T element)
    {
        data[index] = element;
    };
    T &get(int index)
    {
        return static_cast<T &>(data[index]);
    };
    T &operator[](int index) const
    {
        return data[index];
    };
};

// ============================================================
// Component
// ============================================================

class IComponent
{
protected:
    static int _id;
};

template <typename T>
class Component : public IComponent
{
public:
    // returns unique id per component type
    static int id()
    {
        const static int component_id = _id++;
        return component_id;
    }
};

class TransformComponent
{
public:
    vec2 position;
    vec2 scale;
    double rotation;
    TransformComponent(vec2 position = vec2{10, 10}, vec2 scale = vec2{1, 1}, double rotation = 0);
};

class RigidBodyComponent
{

public:
    vec2 velocity;
    RigidBodyComponent(vec2 velocity = vec2{0, 0});
};

class SpriteComponent
{
public:
    std::string asset_name;
    int width;
    int height;
    int z_index;
    SDL_Rect src_rect;
    SpriteComponent(std::string asset_name = ""s, int width = 50, int height = 50, int z_index = 0, int src_rect_x = 0, int src_rect_y = 0);
};

// ============================================================
// Entity
// ============================================================
class Entity
{
private:
    int _id{0};
    class Registry *registry;
    Signature component_signature;

public:
    Entity(int id, Registry *registry) : _id(id), registry(registry){};
    void id(const int new_id);
    int id() const;
    bool operator==(const Entity &other) const;
    bool operator!=(const Entity &other) const;
    bool operator>(const Entity &other) const;
    bool operator<(const Entity &other) const;

    template <typename TComponent, typename... TArgs>
    void add_component(TArgs &&...args);
    template <typename TComponent>
    void remove_component();
    template <typename TComponent>
    bool has_component() const;
    template <typename TComponent>
    TComponent &get_component() const;
};

// ============================================================
// System
// ============================================================
class System
{
private:
    Signature component_signature;
    std::vector<Entity> _entities;

public:
    System() = default;
    ~System() = default;

    void add_entity(Entity entity);
    void remove_entity(Entity entity);
    std::vector<Entity> entities() const;
    const Signature &get_component_signature() const;

    template <typename TComponent>
    void require_component();
};

class MovementSystem : public System
{
public:
    MovementSystem();
    void update(const float dt);
};

class RenderSystem : public System
{
public:
    RenderSystem();
    void update(SDL_Renderer *renderer, std::unique_ptr<AssetStore> &asset_store);
};

// ============================================================
// Registry
// ============================================================
class Registry
{
private:
    int num_entities;
    std::set<Entity> entities_to_add;
    std::set<Entity> entities_to_kill;

    std::vector<std::shared_ptr<IPool>> component_pools = std::vector<std::shared_ptr<IPool>>(1000);
    std::vector<Signature> entity_component_signatures = std::vector<Signature>(1000);
    std::unordered_map<std::type_index, std::shared_ptr<System>> systems;

public:
    Entity create_entity();
    void kill_entity(Entity entity);
    void add_system(System system);

    // component management for a specific entity
    template <typename TComponent, typename... TArgs>
    void add_component(Entity &entity, TArgs &&...args);
    template <typename TComponent>
    void remove_component(Entity entity);
    template <typename TComponent>
    bool has_component(Entity entity) const;
    template <typename TComponent>
    TComponent &get_component(Entity entity) const;

    // system management
    template <typename TSystem, typename... TArgs>
    void add_system(TArgs &&...args);
    template <typename TSystem>
    void remove_system();
    template <typename TSystem>
    bool has_system() const;
    template <typename TSystem>
    TSystem &get_system() const;

    // check for entity component signature and add the entity to all systems that match the component
    void add_entity_to_systems(Entity entity);
    void update();
};

// ============================================================
// implement templated member functions
// ============================================================

// Entity
template <typename TComponent, typename... TArgs>
void Entity::add_component(TArgs &&...args)
{
    registry->add_component<TComponent>(*this, std::forward<TArgs>(args)...);
}

template <typename TComponent>
void Entity::remove_component()
{
    registry->remove_component<TComponent>(*this);
}

template <typename TComponent>
bool Entity::has_component() const
{
    return registry->has_component<TComponent>(*this);
}

template <typename TComponent>
TComponent &Entity::get_component() const
{
    return registry->get_component<TComponent>(*this);
}

// Registry
template <typename TComponent, typename... TArgs>
void Registry::add_component(Entity &entity, TArgs &&...args)
{
    const auto entity_id = entity.id();
    const auto component_id = Component<TComponent>::id();
    if (component_id > component_pools.size())
    {
        component_pools.resize(component_id + 1, nullptr);
    }

    if (!component_pools[component_id])
    {
        auto new_component_pool_ptr = std::make_shared<Pool<TComponent>>();
        component_pools[component_id] = new_component_pool_ptr;
    }

    auto component_pool_ptr = std::static_pointer_cast<Pool<TComponent>>(component_pools[component_id]);

    if (entity_id > component_pool_ptr->size())
    {
        component_pool_ptr->resize(num_entities);
    }

    // create new component
    TComponent new_component(std::forward<TArgs>(args)...);

    // add component to the pool, use entity id as index
    component_pool_ptr->set(entity_id, new_component);

    // set entity signature
    entity_component_signatures.at(entity_id).set(component_id);

    // Logger::info("added component " + std::to_string(component_id) + " to entity " + std::to_string(entity_id));
};
template <typename TComponent>
void Registry::remove_component(Entity entity)
{
    const auto entity_id = entity.id();
    const auto component_id = Component<TComponent>::id();
    entity_component_signatures.at(entity_id).set(component_id, false);

    // Logger::info("remove component " + std::to_string(component_id) + " from entity " + std::to_string(entity_id));
};
template <typename TComponent>
bool Registry::has_component(Entity entity) const
{
    const auto entity_id = entity.id();
    const auto component_id = Component<TComponent>::id();
    return entity_component_signatures.at(entity_id).test(component_id);
};

template <typename TComponent>
TComponent &Registry::get_component(Entity entity) const
{
    const auto entity_id = entity.id();
    const auto component_id = Component<TComponent>::id();
    auto component_pool = std::static_pointer_cast<Pool<TComponent>>(component_pools.at(component_id));

    return component_pool->get(entity_id);
};

// System
template <typename TComponent>
void System::require_component()
{
    component_signature.set(Component<TComponent>::id());
}

// system management
template <typename TSystem, typename... TArgs>
void Registry::add_system(TArgs &&...args)
{
    std::shared_ptr<TSystem> new_system_ptr = std::make_shared<TSystem>(std::forward<TArgs>(args)...);
    systems.insert(std::make_pair(std::type_index(typeid(TSystem)), new_system_ptr));
};
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

#endif