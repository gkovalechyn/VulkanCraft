#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 uv;
layout(location = 3) in vec3 inColor;

layout(set = 0, binding = 0) uniform ModelData {
    mat4 model;
} model;

layout(push_constant) uniform CameraConstants{
    mat4 viewAndProjection;
} camera;

layout(location = 0) out vec4 fragColor;

out gl_PerVertex {
    vec4 gl_Position;
};

void main() {
    gl_Position = camera.viewAndProjection * model.model * vec4(position, 1.0);
    fragColor = vec4(normal, 1.0);
}