#include "lib/stdio.glsl"
#include "lib/stdlib.glsl"

void main()
{
    FRAG_TexCoord = VERT_TexCoord;
    FRAG_Normal   = VERT_Normal;
    gl_Position   = Camera.Projection * Camera.View * Object.Model * vec4(VERT_Position, 1);
}
