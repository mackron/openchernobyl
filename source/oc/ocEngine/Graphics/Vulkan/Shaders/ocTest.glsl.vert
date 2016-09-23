#version 450 core

layout(location = 0) in vec3 IN_Position;
layout(location = 1) in vec2 IN_TexCoord;
layout(location = 2) in vec3 IN_Normal;

layout(location = 0) out vec2 OUT_TexCoord;
layout(location = 1) out vec3 OUT_Normal;

layout (set = 0, binding = 0) uniform UBO_Camera
{
	mat4 projection;
	mat4 view;
} camera;

layout (set = 0, binding = 1) uniform UBO_Object
{
    mat4 model;
} object;

void main()
{
    OUT_TexCoord = IN_TexCoord;
    OUT_Normal = IN_Normal;
    gl_Position = camera.projection * camera.view * object.model * vec4(IN_Position, 1);
}