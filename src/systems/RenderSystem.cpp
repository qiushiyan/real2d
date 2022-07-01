#include "ECS.hpp"
#include "Store.hpp"

RenderSystem::RenderSystem()
{
    require_component<TransformComponent>();
    require_component<SpriteComponent>();
}

void RenderSystem::update(SDL_Renderer *renderer, std::unique_ptr<AssetStore> &asset_store)
{
    for (const auto &entity : entities())
    {
        auto &transform = entity.get_component<TransformComponent>();
        auto &sprite = entity.get_component<SpriteComponent>();

        // source rectangle and dest rectangle
        SDL_Rect src_rect = sprite.src_rect;

        SDL_Rect dest_rect = {(int)transform.position.x,
                              (int)transform.position.y,
                              (int)(sprite.width * transform.scale.x),
                              (int)(sprite.height * transform.scale.y)};

        SDL_RenderCopyEx(renderer, asset_store->get_texture(sprite.asset_name), &src_rect, &dest_rect, transform.rotation, NULL, SDL_FLIP_NONE);
    }
};