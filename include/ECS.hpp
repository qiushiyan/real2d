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

using constants::Signature;

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
    Pool(int n = 100)
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
    T &get(int index) const
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
private:
    glm::vec2 position;
    glm::vec2 scale;
    double rotation;

public:
    TransformComponent(
        glm::vec2 position = glm::vec2{2, 0},
        glm::vec2 scale = glm::vec2{1, 1},
        double rotation = 0)
    {
        this->position = position;
        this->scale = scale;
        this->rotation = rotation;
    };
};

class RigidBodyComponent
{
private:
    glm::vec2 velocity;

public:
    RigidBodyComponent(glm::vec2 velocity = glm::vec2{0, 0})
    {
        this->velocity = velocity;
    };
};

// ============================================================
// Entity
// ============================================================
class Entity
{
private:
    int _id{0};
    Signature component_signature;

public:
    Entity(int id) : _id(id){};
    void id(const int new_id);
    int id() const;
    bool operator==(const Entity &other) const;
    bool operator!=(const Entity &other) const;
    bool operator>(const Entity &other) const;
    bool operator<(const Entity &other) const;
};

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

template <typename TComponent>
void System::require_component()
{
    component_signature.set(Component<TComponent>::id());
}

class Registry
{
private:
    int num_entities{0};
    std::set<Entity> entities_to_add;
    std::set<Entity> entities_to_kill;

    std::vector<std::shared_ptr<IPool>> component_pools = std::vector<std::shared_ptr<IPool>>(10);
    std::vector<Signature> entity_component_signatures = std::vector<Signature>(100);
    std::unordered_map<std::type_index, std::shared_ptr<System>> systems;

public:
    Entity create_entity();
    void kill_entity(Entity entity);
    void add_system(System system);

    // component management for a specific entity
    template <typename TComponent, typename... TArgs>
    void add_component(Entity &entity, TArgs &&...args)
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
    };
    template <typename TComponent>
    void remove_component(Entity entity)
    {
        const auto entity_id = entity.id();
        const auto component_id = Component<TComponent>::id();
        entity_component_signatures.at(entity_id).set(component_id);
    };
    template <typename TComponent>
    bool has_component(Entity entity) const
    {
        const auto entity_id = entity.id();
        const auto component_id = Component<TComponent>::id();
        return entity_component_signatures.at(entity_id).test(component_id);
    };

    // system management
    template <typename TSystem, typename... TArgs>
    void add_system(TArgs &&...args)
    {
        std::shared_ptr<TSystem> new_system_ptr = std::make_shared<TSystem>(std::forward<TArgs>(args)...);
        systems.insert(std::make_pair(std::type_index(typeid(TSystem)), new_system_ptr));
    };
    template <typename TSystem>
    void remove_system()
    {
        auto it = systems.find(std::type_index(typeid(TSystem)));
        systems.erase(it);
    };
    template <typename TSystem>
    bool has_system() const
    {
        return systems.find(std::type_index(typeid(TSystem))) != systems.end() ? true : false;
    };
    template <typename TSystem>
    TSystem &get_system() const
    {
        auto it = systems.find(std::type_index(typeid(TSystem)));
        return *(std::static_pointer_cast<TSystem>(it->second));
    };

    // check for entity component signature and add the entity to all systems that match the component
    void add_entity_to_systems(Entity entity);
    void update();
};

#endif