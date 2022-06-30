#include "ECS.hpp"

TransformComponent::TransformComponent(vec2 position, vec2 scale, double rotation)
{
    this->position = position;
    this->scale = scale;
    this->rotation = rotation;
};