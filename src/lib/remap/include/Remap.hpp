#ifndef REMAP_H
#define REMAP_H

float Lerp(float a, float b, float t);
float InvLerp(float a, float b, float v);
float Remap(float i_min, float i_max, float o_min, float o_max, float v);

#endif