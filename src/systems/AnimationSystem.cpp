#include "ECS.hpp"

AnimationSystem::AnimationSystem()
{
    require_component<SpriteComponent>();
    require_component<AnimationComponent>();
}

void AnimationSystem::update()
{
    for (const auto &entity : entities())
    {
        auto &sprite = entity.get_component<SpriteComponent>();
        auto &animation = entity.get_component<AnimationComponent>();

        animation.current_frame = ((SDL_GetTicks() - animation.start_time) * animation.frame_rate / 1000) % animation.num_frames;
        sprite.src_rect.x = animation.current_frame * sprite.width;
    }
}