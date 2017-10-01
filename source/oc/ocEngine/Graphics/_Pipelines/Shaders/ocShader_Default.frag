#include "lib/stdio.glsl"
#include "lib/stdlib.glsl"

void main()
{
    OUT_Color = texture(Texture0, FRAG_TexCoord, 0.0f);

#ifndef VULKAN
    gl_FragColor = OUT_Color;
#endif
}
