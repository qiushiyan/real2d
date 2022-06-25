#ifndef ECS_H
#define ECS_H
#include "constants.hpp"
#include <vector>
#include <unordered_map>
#include <typeindex>
#include <set>
#include <glm/glm.hpp>

using constants::Signature;

// ============================================================
// Pool
// ============================================================

class IPool
{
public:
};

template <typename T>
class Pool : IPool
{
private:
    std::vector<T> data;

public:
    bool empty() const;
    bool size() const;
    void clear();
    void resize(std::uint8_t n);
    void add(T element);
    void set(std::uint8_t, T element);
    T &get(std::uint8_t index) const;
    T &operator[](std::uint8_t index) const;
};

// ============================================================
// Component
// ============================================================

class IComponent
{
protected:
    static std::uint8_t id;
};

template <typename T>
class Component : public IComponent
{
public:
    // returns unique id per component type
    static std::uint8_t id()
    {
        const static std::uint8_t component_id = id++;
        return component_id;
    };
};

class TransformComponent
{
public:
    glm::vec2 position;
    glm::vec2 scale;
    double rotation;
    TransformComponent(
        glm::vec2 position,
        glm::vec2 scale,
        double rotation)
    {
    }
};

// ============================================================
// Entity
// ============================================================
class Entity
{
private:
    std::uint8_t _id{0};
    Signature component_signature;

public:
    Entity(std::uint8_t id) : _id(id){};
    void id(const std::uint8_t new_id);
    std::uint8_t id() const;
    bool operator==(const Entity &other) const;
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
    std::size_t num_entities{0};
    std::set<Entity> entities_to_add;
    std::set<Entity> entities_to_kill;

    std::vector<IPool *> component_pools;
    std::vector<Signature> entity_component_signatures;
    std::unordered_map<std::type_index, System *> systems;

public:
    Entity create_entity();
    void kill_entity(Entity entity);
    void add_system(System system);

    // component management for a specific entity
    template <typename TComponent, typename... TArgs>
    void add_component(Entity entity, TArgs &&...);
    template <typename TComponent>
    void remove_component(Entity entity);
    template <typename TComponent>
    bool has_component(Entity entity) const;

    // system management
    template <typename TSystem, typename... TArgs>
    void add_system(TArgs &&...);
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

#endif