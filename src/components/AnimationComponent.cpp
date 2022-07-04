#include "ECS.hpp"
#include "SDL2/SDL.h"

AnimationComponent::AnimationComponent(int num_frames, int frame_rate, bool should_loop)
{
    this->num_frames = num_frames;
    this->current_frame = 1;
    this->frame_rate = frame_rate;
    this->should_loop = should_loop;
    this->start_time = SDL_GetTicks();
}
