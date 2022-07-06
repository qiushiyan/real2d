#ifndef ECS_H
#define ECS_H
#include <vector>
#include <unordered_map>
#include <set>
#include <deque>
#include <glm/glm.hpp>
#include <memory>
#include <iostream>
#include <Logger.hpp>
#include <SDL2/SDL.h>
#include <typeindex>
#include <functional>
#include <list>
#include "constants.hpp"
#include "Store.hpp"

using constants::Signature;
using glm::vec2;
using namespace std::string_literals;

// ============================================================
// Pool
// ============================================================

class IPool
{
public:
    virtual ~IPool() = default;
    virtual void remove(int entity_id) = 0;
};

template <typename T>
class Pool : public IPool
{
private:
    std::vector<T> data;
    int n_entities;

    std::unordered_map<int, int> entity_to_index;
    std::unordered_map<int, int> index_to_entity;

public:
    Pool(int capacity = 100)
    {
        n_entities = 0;
        data.resize(capacity);
    };
    ~Pool()
    {
        data.clear();
        entity_to_index.clear();
        index_to_entity.clear();
        n_entities = 0;
    };

    bool is_empty() const
    {
        return n_entities == 0;
    };
    std::size_t size() const
    {
        return n_entities;
    };
    void clear()
    {
        n_entities = 0;
        data.clear();
    };
    void resize(int capacity)
    {
        data.resize(capacity);
    };
    void set(int entity_id, T element)
    {
        if (entity_to_index.find(entity_id) != entity_to_index.end())
        {
            int index = entity_to_index[entity_id];
            data[index] = element;
        }

        int index = n_entities;
        entity_to_index.emplace(entity_id, index);
        index_to_entity.emplace(index, entity_id);

        if (index >= data.capacity())
        {
            data.resize(n_entities * 2);
        }

        data[index] = element;

        n_entities++;
    };

    virtual void remove(int entity_id) override
    {
        if (entity_to_index.find(entity_id) == entity_to_index.end())
        {
            return;
        }
        int index_of_removed = entity_to_index[entity_id];
        // replace content with last element
        int last_index = n_entities - 1;
        data[index_of_removed] = data[last_index];

        int last_index_entity_id = index_to_entity[last_index];

        entity_to_index[last_index_entity_id] = index_of_removed;
        index_to_entity[index_of_removed] = last_index_entity_id;

        entity_to_index.erase(entity_id);
        index_to_entity.erase(last_index);
        n_entities--;
    }
    T &get(int entity_id)
    {
        int index = entity_to_index[entity_id];
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
    RigidBodyComponent(vec2 velocity = vec2(0));
};

class SprintComponent
{
public:
    bool in_sprint;
    float sprint_speed;
    int sprint_duration;
    int sprint_cooldown;
    int last_sprint_time;
    SprintComponent(bool in_sprit = false, float sprint_speed = 2, int sprint_duration = 3000, int sprint_cooldown = 5000);
};

class SpriteComponent
{
public:
    std::string asset_name;
    int width;
    int height;
    int z_index;
    bool is_fixed;
    SDL_Rect src_rect;
    SpriteComponent(std::string asset_name = ""s, int width = 32, int height = 32, int z_index = 0, bool is_fixed = false, int src_rect_x = 0, int src_rect_y = 0);
};

class AnimationComponent
{
public:
    int current_frame;
    int num_frames; // number of frames in the animation
    int frame_rate; // how many frame per second
    bool should_loop;
    int start_time; // current ticks
    AnimationComponent(int num_frames = 1, int frame_rate = 1, bool should_loop = true);
};

class BoxColliderComponent
{
public:
    int width;
    int height;
    vec2 offset;
    BoxColliderComponent(int width = 0, int height = 0, vec2 offset = vec2(0));
};

class KeyboardControlComponent
{
public:
    vec2 up_velocity;
    vec2 right_velocity;
    vec2 down_velocity;
    vec2 left_velocity;
    KeyboardControlComponent(vec2 up_velocity = vec2(0), vec2 right_velocity = vec2(0), vec2 down_velocity = vec2(0), vec2 left_velocity = vec2(0));
};

class MouseControlComponent
{
public:
    MouseControlComponent();
};

class CameraFollowComponent
{
public:
    CameraFollowComponent();
};

class ProjectileEmitterComponent
{
public:
    vec2 velocity;
    int freq;     // emitting frequency, ticks per emission
    int duration; // duration of the projectile
    bool is_friendly;
    int damage; // damage of the projectile, in percentage
    int last_emission_time;

    ProjectileEmitterComponent(vec2 velocity = vec2(0), int freq = 500, int duration = 5000, bool is_friendly = true, int damage = 20);
};

class ProjectileComponent
{
public:
    int duration;
    bool is_friendly;
    int damage;
    int start_time;

    ProjectileComponent(int duration = 5000, bool is_friendly = true, int damage = 20);
};

class HealthComponent
{
public:
    int health;
    HealthComponent(int health = 100);
};

class TextComponent
{
public:
    vec2 position;
    std::string text;
    std::string font_name;
    SDL_Color color;
    bool is_fixed;
    TextComponent(vec2 position = vec2(0), std::string text = "", std::string font_name = "", const SDL_Color color = {0, 0, 0}, bool is_fixed = true);
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
    class Registry *registry;
    Entity(int id, Registry *registry) : _id(id), registry(registry){};
    void id(const int new_id);
    int id() const;
    void kill() const;
    bool operator==(const Entity &other) const;
    bool operator!=(const Entity &other) const;
    bool operator>(const Entity &other) const;
    bool operator<(const Entity &other) const;

    void tag(const std::string &tag);
    bool has_tag(const std::string &tag) const;
    void group(const std::string &group);
    bool belongs_to_group(const std::string &group) const;

    template <typename TComponent, typename... TArgs>
    void add_component(TArgs &&...args);
    template <typename TComponent>
    void remove_component();
    template <typename TComponent>
    bool has_component() const;
    template <typename TComponent>
    TComponent &get_component() const;
};

class Event
{
};

class CollisionEvent : public Event
{
public:
    Entity a;
    Entity b;
    CollisionEvent(Entity a, Entity b);
};

class KeyPressedEvent : public Event
{
public:
    SDL_Keycode key;
    KeyPressedEvent(SDL_Keycode key);
};

class MouseClickedEvent : public Event
{
public:
    int button;
    MouseClickedEvent(int button);
};

class IEventCallback
{
private:
    virtual void call(Event &e) = 0;

public:
    virtual ~IEventCallback() = default;
    void exec(Event &e)
    {
        call(e);
    }
};

template <typename TOwner, typename TEvent>
class EventCallback : public IEventCallback
{
private:
    typedef void (TOwner::*Callback)(TEvent &);
    TOwner *owner;
    Callback callback;

    virtual void call(Event &e) override
    {
        std::invoke(callback, owner, static_cast<TEvent &>(e));
    }

public:
    virtual ~EventCallback() override = default;
    EventCallback(TOwner *owner, Callback callback)
    {
        this->owner = owner;
        this->callback = callback;
    };
    ;
};

using HandlerList = std::list<std::unique_ptr<IEventCallback>>;

class EventBus
{
private:
    std::map<std::type_index, std::unique_ptr<HandlerList>> subscribers;

public:
    void reset()
    {
        subscribers.clear();
    };
    template <typename TEvent, typename... TArgs>
    void emit(TArgs &&...args);
    template <typename TOwner, typename TEvent>
    void subscribe(TOwner *owner, void (TOwner::*callback)(TEvent &));
};

// ============================================================
// System
// ============================================================
class System
{
private:
    Signature component_signature;

protected:
    std::vector<Entity> _entities;

public:
    System() = default;
    virtual ~System() = default;

    virtual void add_entity(Entity entity);
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
    void add_entity(Entity entity) override final;
    void update(SDL_Renderer *renderer, std::shared_ptr<AssetStore> asset_store, SDL_Rect &camera);
};

class AnimationSystem : public System
{
public:
    AnimationSystem();
    void update();
};

class RenderTextSystem : public System
{
public:
    RenderTextSystem();
    void update(SDL_Renderer *renderer, std::shared_ptr<AssetStore> asset_store, const SDL_Rect &camera);
};

class RenderHealthSystem : public System
{
public:
    RenderHealthSystem();
    void update(SDL_Renderer *renderer, std::shared_ptr<AssetStore> asset_store, SDL_Rect &camera);
};

class CollisionSystem : public System
{
public:
    CollisionSystem();
    bool collide(const Entity &entity, const Entity &other);
    void update(std::shared_ptr<EventBus> event_bus);
};

class RenderColliderSystem : public System
{
public:
    RenderColliderSystem();
    void update(SDL_Renderer *renderer, SDL_Rect &camera);
};

class DamageSystem : public System
{
public:
    DamageSystem();
    void subscribe_events(std::shared_ptr<EventBus> event_bus);
    void on_collision(CollisionEvent &e);
    void on_projectile_hit_player(Entity &projectile, Entity &player);
    void on_projectile_hit_enemy(Entity &projectile, Entity &enemy);
    void update();
};

class KeyboardControlSystem : public System
{
public:
    KeyboardControlSystem();
    void subscribe_events(std::shared_ptr<EventBus> event_bus);
    void on_key_pressed(KeyPressedEvent &e);
    void update();
};

class MouseControlSystem : public System
{
public:
    MouseControlSystem();
    void subscribe_events(std::shared_ptr<EventBus> event_bus);
    void on_mouse_clicked(MouseClickedEvent &e);
    void update();
};

class CameraMovementSystem : public System
{
public:
    CameraMovementSystem();
    void update(SDL_Rect &camera);
};

class Registry;
class ProjectileEmitSystem : public System
{
public:
    ProjectileEmitSystem();
    void on_key_pressed(KeyPressedEvent &e);
    void on_mouse_clicked(MouseClickedEvent &e);
    void subscribe_events(std::shared_ptr<EventBus> event_bus);
    void emit_from(const Entity &entity, bool const_direction);
    void update(std::shared_ptr<Registry> registry);
};

class ProjectileLifecycleSystem : public System
{
public:
    ProjectileLifecycleSystem();
    void update();
};

class RenderGuiSystem : public System
{
public:
    RenderGuiSystem();
    void update(std::shared_ptr<Registry> registry, SDL_Rect &camera);
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
    std::deque<int> free_ids;

    // one unique tag per entity
    std::unordered_map<int, std::string> tag_per_entity;
    std::unordered_map<std::string, Entity> entity_per_tag;

    // one group maps to many groups
    std::unordered_map<std::string, std::set<Entity>> entities_per_group;
    std::unordered_map<int, std::string> group_per_entity;

public:
    Entity create_entity();
    void kill_entity(Entity entity);

    // component management for a specific entity
    template <typename TComponent, typename... TArgs>
    void add_component(Entity &entity, TArgs &&...args);
    template <typename TComponent>
    void remove_component(Entity entity);
    template <typename TComponent>
    bool has_component(Entity entity) const;
    template <typename TComponent>
    TComponent &get_component(Entity entity) const;

    // tag and group management
    void tag(Entity entity, const std::string &tag);
    bool has_tag(Entity entity, const std::string &tag) const;
    Entity get_entity_by_tag(const std::string &tag) const;
    void remove_entity_tag(Entity entity);

    void group(Entity entity, const std::string &group);
    bool belongs_to_group(Entity entity, const std::string &group) const;
    std::vector<Entity> get_entities_by_group(const std::string &group) const;
    void remove_entity_group(Entity entity);

    // system management
    template <typename TSystem, typename... TArgs>
    void add_system(TArgs &&...args);
    template <typename TSystem>
    void remove_system();
    template <typename TSystem>
    bool has_system() const;
    template <typename TSystem>
    TSystem &get_system() const;

    // check for entity component signature and add / remove the entity to all systems that match the component
    void add_entity_to_systems(Entity entity);
    void remove_entity_from_systems(Entity entity);
    void update();
};

// ============================================================
// templated member function implementations
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

// EventBus

template <typename TEvent, typename... TArgs>
void EventBus::emit(TArgs &&...args)
{
    auto handlers = subscribers[std::type_index(typeid(TEvent))].get();
    if (handlers)
    {
        for (auto it = handlers->begin(); it != handlers->end(); ++it)
        {
            auto handler = it->get();
            TEvent event(std::forward<TArgs>(args)...);
            handler->exec(event);
        }
    }
};

template <typename TOwner, typename TEvent>
void EventBus::subscribe(TOwner *owner, void (TOwner::*callback)(TEvent &))
{
    if (!subscribers[typeid(TEvent)].get())
    {
        subscribers[typeid(TEvent)] = std::make_unique<HandlerList>();
    }
    auto handler = std::make_unique<EventCallback<TOwner, TEvent>>(owner, callback);
    subscribers[typeid(TEvent)]->push_back(std::move(handler));
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
    // remove component from the entity's component signature
    const auto entity_id = entity.id();
    const auto component_id = Component<TComponent>::id();
    entity_component_signatures.at(entity_id).set(component_id, false);

    // remove component instance from the corresponding component pool
    std::shared_ptr<Pool<TComponent>> component_pool_ptr = std::static_pointer_cast<Pool<TComponent>>(component_pools[component_id]);

    component_pool_ptr->remove(entity_id);

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
    if (it == systems.end())
    {
        throw std::runtime_error("System not found");
    }
    return *(std::static_pointer_cast<TSystem>(it->second));
};

#endif