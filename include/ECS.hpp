#ifndef ECS_H
#define ECS_H
#include "constants.hpp"
#include <vector>
#include <unordered_map>
#include <typeindex>
#include <set>
#include <glm/glm.hpp>
#include <memory>

using constants::Signature;

// ============================================================
// Pool
// ============================================================

class IPool
{
};

template <typename T>
class Pool : IPool
{
private:
    std::vector<T> data;

public:
    Pool(std::uint8_t n = 100)
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
    bool size() const
    {
        return data.size();
    };
    void clear()
    {
        data.clear();
    };
    void resize(std::uint8_t n)
    {
        data.resize(n);
    };
    void add(T element)
    {
        data.push_back(element);
    };
    void set(std::uint8_t, T element)
    {
        data[index] = element;
    };
    T &get(std::uint8_t index) const
    {
        return static_cast<T &>(data[index]);
    };
    T &operator[](std::uint8_t index) const
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
    static std::uint8_t _id;
};

template <typename T>
class Component : public IComponent
{
public:
    // returns unique id per component type
    static int id()
    {
        const static std::uint8_t component_id = _id++;
        return component_id;
    }
};

class TransformComponent
{
public:
    TransformComponent(
        glm::vec2 position = glm::vec2{2, 0},
        glm::vec2 scale = glm::vec2{1, 1},
        double rotation = 0) : position(position),
                               scale(scale), rotation(rotation){};
    glm::vec2 position;
    glm::vec2 scale;
    double rotation;
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
    std::uint8_t num_entities{0};
    std::set<Entity> entities_to_add;
    std::set<Entity> entities_to_kill;

    std::vector<std::shared_ptr<IPool>> component_pools;
    std::vector<Signature> entity_component_signatures;
    std::unordered_map<std::type_index, std::shared_ptr<System>> systems;

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