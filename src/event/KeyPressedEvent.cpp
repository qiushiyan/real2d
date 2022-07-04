#include "ECS.hpp"

KeyPressedEvent::KeyPressedEvent(SDL_Keycode key)
{
    this->key = key;
};