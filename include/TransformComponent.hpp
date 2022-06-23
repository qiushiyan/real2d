#ifndef TRANSFORM_COMPONENT_H
#define TRANSFORM_COMPONENT_H

#include <glm/glm.hpp>

class TransformComponent
{
public:
    glm::vec2 position;
    glm::vec2 scale;
    double rotation;
};

#endif