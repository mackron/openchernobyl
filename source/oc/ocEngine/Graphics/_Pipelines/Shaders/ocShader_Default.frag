#include "lib/stdlib.glsl"
#include "lib/stdio.glsl"

void main()
{
    OUT_Color = texture(Texture0, FRAG_TexCoord, 0.0f);

#ifndef VULKAN
    gl_FragColor = OUT_Color;
#endif
}
