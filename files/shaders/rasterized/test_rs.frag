#version 460

layout(location = 0) in vec2 inUV;

layout(location = 0) out vec4 outColor;

layout(push_constant) uniform TestData
{
    layout(offset = 0, row_major) mat4 modelMatrix;
    layout(offset = 64)           vec4 color;
    layout(offset = 80)           uint debugMode;
} testData;

void main()
{
    if (testData.debugMode != 0)
    {
        outColor = vec4(
            inUV.x,
            inUV.y,
            0.5,
            1.0);
    }
    else outColor = testData.color;
}
