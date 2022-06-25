#include "ECS.hpp"

TransformComponent::TransformComponent(
    glm::vec2 position = glm::vec2(2, 0),
    glm::vec2 scale = glm::vec2(1, 1),
    double rotation = 0) : position(position),
                           scale(scale), rotation(rotation)
{
}