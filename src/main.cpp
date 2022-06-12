#include <iostream>
#include <lua.hpp>
#include <glm/glm.hpp>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#include <imgui.h>
#include <sol/sol.hpp>

int main()
{
    sol::state lua;
    lua.open_libraries(sol::lib::base);
    SDL_Init(SDL_INIT_EVERYTHING);
    glm::vec2 velocity = glm::vec2(5.0, 1.0);
    std::cout << "hello world" << std::endl;
}
