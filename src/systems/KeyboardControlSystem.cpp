#include "ECS.hpp"

KeyboardControlSystem::KeyboardControlSystem()
{
    require_component<KeyboardControlComponent>();
    require_component<RigidBodyComponent>();
    require_component<SpriteComponent>();
}

void KeyboardControlSystem::on_key_pressed(KeyPressedEvent &e)
{
    for (const auto &entity : entities())
    {
        const auto &keyboard = entity.get_component<KeyboardControlComponent>();
        auto &rigid_body = entity.get_component<RigidBodyComponent>();
        auto &sprite = entity.get_component<SpriteComponent>();

        switch (e.key)
        {
        case SDLK_w:
            rigid_body.velocity = keyboard.up_velocity;
            sprite.src_rect.y = sprite.height * 0;
            break;
        case SDLK_d:
            rigid_body.velocity = keyboard.right_velocity;
            sprite.src_rect.y = sprite.height * 1;
            break;
        case SDLK_s:
            rigid_body.velocity = keyboard.down_velocity;
            sprite.src_rect.y = sprite.height * 2;
            break;
        case SDLK_a:
            rigid_body.velocity = keyboard.left_velocity;
            sprite.src_rect.y = sprite.height * 3;
            break;
        }
    }
}

void KeyboardControlSystem::subscribe_events(std::shared_ptr<EventBus> event_bus)
{
    event_bus->subscribe(this, &KeyboardControlSystem::on_key_pressed);
}

void KeyboardControlSystem::update()
{
}