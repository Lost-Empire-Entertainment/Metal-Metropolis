#version 460

layout(location = 0) in vec3 inNormal;
layout(location = 1) in vec2 inUV;

layout(location = 0) out vec4 outColor;

layout(push_constant) uniform PushConstants
{
    layout(offset = 0) vec4 color;
    layout(offset = 16) uint debugMode;
} pc;

void main()
{
    if (pc.debugMode != 0) outColor = vec4(inUV.x, inUV.y, 0.5, 1.0);
    else outColor = pc.color;
}
