#include "ECS.hpp"
#include "glm/glm.hpp"

RigidBodyComponent::RigidBodyComponent(glm::vec2 velocity)
{
    this->velocity = velocity;
};