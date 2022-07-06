#include <Remap.hpp>

float Lerp(float a, float b, float t)
{
    return (1.0f - t) * a + b * t;
}

float InvLerp(float a, float b, float v)
{
    return (v - a) / (b - a);
}

float Remap(float i_min, float i_max, float o_min, float o_max, float v)
{
    float t = InvLerp(i_min, i_max, v);
    return Lerp(o_min, o_max, t);
}
