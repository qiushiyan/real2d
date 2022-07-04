#include "ECS.hpp"

KeyboardControlSystem::KeyboardControlSystem()
{
}

void KeyboardControlSystem::on_key_pressed(KeyPressedEvent &e)
{
    std::string keyCode = std::to_string(e.key);
    std::cout << keyCode << std::endl;
}

void KeyboardControlSystem::subscribe_events(std::shared_ptr<EventBus> event_bus)
{
    event_bus->subscribe(this, &KeyboardControlSystem::on_key_pressed);
}

void KeyboardControlSystem::update()
{
}