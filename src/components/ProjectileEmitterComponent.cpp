#include "ECS.hpp"

ProjectileEmitterComponent::ProjectileEmitterComponent(vec2 velocity, int freq, int duration, bool is_friendly, int damage)
{
    this->velocity = velocity;
    this->freq = freq;
    this->duration = duration;
    this->is_friendly = is_friendly;
    this->damage = damage;
    this->last_emission_time = SDL_GetTicks();
}