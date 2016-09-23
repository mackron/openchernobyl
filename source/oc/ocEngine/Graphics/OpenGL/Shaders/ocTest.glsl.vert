#version 120

uniform mat4 Projection;
uniform mat4 View;
uniform mat4 Model;

varying vec2 TexCoord;
varying vec3 Normal;

void main()
{
	TexCoord = gl_MultiTexCoord0.xy;
	Normal = gl_Normal;
    gl_Position = Projection * View * Model * gl_Vertex;
}