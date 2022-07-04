#include "ECS.hpp"

ProjectileComponent::ProjectileComponent(
    int duration, bool is_friendly, int damage)
{
    this->duration = duration;
    this->is_friendly = is_friendly;
    this->damage = damage;
    this->start_time = SDL_GetTicks();
};
