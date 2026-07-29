#version 460

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inUV;

layout(set = 0, binding = 0) uniform TransformUBO
{
    mat4 modelMatrix;
} transform;

layout(set = 0, binding = 1) uniform CameraUBO
{
    mat4 viewProjection;
} camera;

layout(location = 0) out vec2 outUV;

void main()
{
    gl_Position =
        camera.viewProjection
        * transform.modelMatrix
        * vec4(inPosition, 1.0);

    outUV = inUV;
}
