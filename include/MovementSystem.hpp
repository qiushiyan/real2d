#ifndef MOVEMENT_SYSTEM_H
#define MOVEMENT_SYSTEM_H

#include "ECS.hpp"
class MovementSystem : public System
{
public:
    MovementSystem();
    void update();
};

#endif