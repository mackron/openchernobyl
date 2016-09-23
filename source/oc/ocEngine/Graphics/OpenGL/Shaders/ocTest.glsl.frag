#version 120

uniform sampler2D Texture;

varying vec2 TexCoord;
varying vec3 Normal;

void main()
{
    //gl_FragColor = vec4(1, 1, 0, 1);
    gl_FragColor = texture2D(Texture, TexCoord);
}