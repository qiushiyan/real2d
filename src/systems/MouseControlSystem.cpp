#include "ECS.hpp"

MouseControlSystem::MouseControlSystem()
{
    require_component<MouseControlComponent>();
}

void MouseControlSystem::update()
{
}