#version 460

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inUV;

layout(push_constant) uniform TestData
{
    layout(offset = 0)  mat4 modelMatrix;
    layout(offset = 64) vec4 color;
    layout(offset = 80) uint debugMode;
} testData;

layout(set = 0, binding = 0) uniform CameraUBO
{
    mat4 projectionMatrix;
} camera;

layout(location = 0) out vec2 outUV;

void main()
{
    gl_Position =
        camera.projectionMatrix
        * testData.modelMatrix
        * vec4(inPosition, 1.0);

    outUV = inUV;
}
