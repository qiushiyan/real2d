#include <iostream>
#include "Game.hpp"

int main()
{
    Game game{};
    game.init();
    game.setup();
    game.run();
}
