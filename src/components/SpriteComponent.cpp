#include "ECS.hpp"
#include "glm/glm.hpp"

SpriteComponent::SpriteComponent(std::string asset_name, int width, int height, int z_index, bool is_fixed, int src_rect_x, int src_rect_y)
{
    this->asset_name = asset_name;
    this->width = width;
    this->height = height;
    this->z_index = z_index;
    this->is_fixed = is_fixed;
    this->src_rect = {src_rect_x, src_rect_y, width, height};
};