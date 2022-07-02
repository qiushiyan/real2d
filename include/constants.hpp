#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <bitset>

// define your own namespace to hold constants
namespace constants
{
    inline constexpr std::uint16_t windowWidth{1200};
    inline constexpr std::uint16_t windowHeight{800};
    inline constexpr int FPS{60};
    inline constexpr int TICKS_PER_FRAME{1000 / FPS};
    inline constexpr int MAX_COMPONENTS{32};
    inline constexpr int tile_size{32};
    inline constexpr int tile_scale{1};
    inline constexpr int map_rows{20};
    inline constexpr int map_cols{25};

    using Signature = std::bitset<MAX_COMPONENTS>;

}
#endif