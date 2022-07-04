#include "ECS.hpp"

BoxColliderComponent::BoxColliderComponent(int width, int height, vec2 offset)
{
    this->width = width;
    this->height = height;
    this->offset = offset;
};