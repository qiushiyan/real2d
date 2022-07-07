#include "ECS.hpp"
#include "SDL2/SDL.h"

AnimationComponent::AnimationComponent(int num_frames, int normal_frame_rate, bool should_loop)
{
    this->num_frames = num_frames;
    this->current_frame = 1;
    this->normal_frame_rate = normal_frame_rate;
    this->frame_rate = normal_frame_rate;
    this->sprint_frame_rate = normal_frame_rate * 2;
    this->should_loop = should_loop;
    this->start_time = SDL_GetTicks();
}
