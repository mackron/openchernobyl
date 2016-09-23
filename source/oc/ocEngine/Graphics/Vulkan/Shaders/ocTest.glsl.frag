#version 450 core

layout (location = 0) in vec2 IN_TexCoord;
layout (location = 1) in vec3 IN_Normal;

layout (location = 0) out vec4 OUT_Color;

layout (binding = 2) uniform sampler2D Texture0;

void main()
{
    //OUT_Color = vec4(1, 1, 0, 1);
    //OUT_Color = vec4(IN_TexCoord, 0, 1);
    OUT_Color = texture(Texture0, IN_TexCoord, 0);
}