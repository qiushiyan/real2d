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
    // sort entity when inserted
    auto it = std::upper_bound(_entities.begin(), _entities.end(), entity, [](const Entity &entity, const Entity &other)
                               { return entity.get_component<SpriteComponent>().z_index < other.get_component<SpriteComponent>().z_index; });

    _entities.insert(it, entity);
}

void RenderSystem::update(SDL_Renderer *renderer, std::shared_ptr<AssetStore> asset_store, SDL_Rect &camera)
{
    for (const auto &entity : entities())
    {

        auto &transform = entity.get_component<TransformComponent>();
        auto &sprite = entity.get_component<SpriteComponent>();

        // bypass entities outside of camera view
        int padding = 50;

        bool is_outside_camera = (transform.position.x + sprite.width + padding < camera.x ||
                                  transform.position.x > camera.x + camera.w + sprite.width + padding ||
                                  transform.position.y + sprite.height + padding < camera.y ||
                                  transform.position.y > camera.y + camera.h + sprite.height + padding);

        if (!is_outside_camera || sprite.is_fixed)
        {
            // source rectangle and dest rectangle
            SDL_Rect src_rect = sprite.src_rect;

            SDL_Rect dest_rect = {(int)(transform.position.x - (sprite.is_fixed ? 0 : camera.x)),
                                  (int)(transform.position.y - (sprite.is_fixed ? 0 : camera.y)),
                                  (int)(sprite.width * transform.scale.x),
                                  (int)(sprite.height * transform.scale.y)};

            SDL_RenderCopyEx(renderer, asset_store->get_texture(sprite.asset_name), &src_rect, &dest_rect, transform.rotation, NULL, sprite.flip);
        }
    }
};