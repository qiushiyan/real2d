#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <bitset>

// define your own namespace to hold constants
namespace constants
{
    inline constexpr std::uint16_t window_width{1200};
    inline constexpr std::uint16_t window_height{800};
    inline constexpr int FPS{60};
    inline constexpr int TICKS_PER_FRAME{1000 / FPS};
    inline constexpr int MAX_COMPONENTS{32};
    inline constexpr int tile_size{32};
    inline constexpr double tile_scale{2};
    inline constexpr int map_rows{20};
    inline constexpr int map_cols{25};
    inline constexpr double map_width{map_cols * tile_size * tile_scale};
    inline constexpr double map_height{map_rows * tile_size * tile_scale};

    using Signature = std::bitset<MAX_COMPONENTS>;

}
#endif