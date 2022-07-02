#include "ECS.hpp"
#include "Store.hpp"
#include <algorithm>

RenderSystem::RenderSystem()
{
    require_component<TransformComponent>();
    require_component<SpriteComponent>();
}

class RenderableEntity
{
public:
    RenderableEntity(const TransformComponent &transform, const SpriteComponent &sprite) : transform_component(transform), sprite_component(sprite){};
    TransformComponent transform_component;
    SpriteComponent sprite_component;
};

// override System::add_entity and sort by z-index
void RenderSystem::add_entity(Entity entity)
{
    auto it = std::upper_bound(_entities.begin(), _entities.end(), entity, [](const Entity &entity, const Entity &other)
                               { return entity.get_component<SpriteComponent>().z_index < other.get_component<SpriteComponent>().z_index; });

    _entities.insert(it, entity);
}

void RenderSystem::update(SDL_Renderer *renderer, std::unique_ptr<AssetStore> &asset_store)
{
    // std::vector<RenderableEntity> renderable_entities;
    // for (const auto &entity : entities())
    // {
    //     renderable_entities.emplace_back(entity.get_component<TransformComponent>(),
    //                                      entity.get_component<SpriteComponent>());
    // }

    // std::sort(renderable_entities.begin(), renderable_entities.end(),
    //           [](const RenderableEntity &a, const RenderableEntity &b)
    //           { return a.sprite_component.z_index < b.sprite_component.z_index; });

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