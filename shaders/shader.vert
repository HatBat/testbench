#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
} ubo;

layout(location = 0) in vec4 inPosition;

layout(location = 0) out vec3 fragColor;

void main() {
	gl_Position = ubo.proj * ubo.view * ubo.model * vec4(inPosition.xy, 0.0, 1.0);
    //gl_Position = inPosition;
    fragColor = vec3(1.0, 1.0, 1.0);
}