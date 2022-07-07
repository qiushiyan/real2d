#include "ECS.hpp"

ScriptComponent::ScriptComponent(sol::function fun)
{
    this->fun = fun;
}