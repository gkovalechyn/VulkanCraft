#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 uv;
layout(location = 3) in vec3 inColor;

layout(push_constant) uniform CameraConstants {
    mat4 viewAndProjection;
} camera;

layout(location = 0) out vec4 fragColor;

out gl_PerVertex {
    vec4 gl_Position;
};

void main() {
    gl_Position = camera.viewAndProjection * vec4(position, 1.0);

    fragColor = vec4(inColor, 1.0);
}