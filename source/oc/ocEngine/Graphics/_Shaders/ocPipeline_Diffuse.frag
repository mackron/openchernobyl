#include "lib/stdlib.glsl"
#include "lib/stdio.glsl"

vec4 Material_Diffuse();

void main()
{
    OUT_Color = Material_Diffuse();
    
#ifdef OPENGL
    gl_FragColor = OUT_Color;
#endif
}

//# <<FRAG_Material>>
