#ifndef CONSTANTS_H
#define CONSTANTS_H

// define your own namespace to hold constants
namespace constants
{
    inline constexpr int windowWidth{1200};
    inline constexpr int windowHeight{800};
    inline constexpr int FPS{60};
    inline constexpr int TICKS_PER_FRAME{1000 / FPS};
}
#endif