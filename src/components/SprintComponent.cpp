#include "ECS.hpp"

SprintComponent::SprintComponent(bool in_sprint, float sprint_speed, int sprint_duration, int sprint_cooldown)
{
    this->in_sprint = in_sprint;
    this->sprint_speed = sprint_speed;
    this->sprint_duration = sprint_duration;
    this->sprint_cooldown = sprint_cooldown;
    this->last_sprint_time = SDL_GetTicks() - sprint_cooldown - sprint_duration;
}