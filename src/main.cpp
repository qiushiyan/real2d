#include <iostream>
#include <Remap.hpp>
#include "Game.hpp"

int main()
{
    Game game{};
    game.init();
    game.setup();
    game.run();
}
