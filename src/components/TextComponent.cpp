#include "ECS.hpp"

TextComponent::TextComponent(vec2 position, std::string text, std::string font_name, const SDL_Color color, bool is_fixed)
{
    this->position = position;
    this->text = text;
    this->font_name = font_name;
    this->color = color;
    this->is_fixed = is_fixed;
}
