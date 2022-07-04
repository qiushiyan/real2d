#include "ECS.hpp"

KeyboardControlComponent::KeyboardControlComponent(vec2 up_velocity, vec2 right_velocity, vec2 down_velocity, vec2 left_velocity)
{
    this->up_velocity = up_velocity;
    this->right_velocity = right_velocity;
    this->down_velocity = down_velocity;
    this->left_velocity = left_velocity;
};