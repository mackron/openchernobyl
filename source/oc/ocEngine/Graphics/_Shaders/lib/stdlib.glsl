
#define OC_UNIFORM(_binding) layout (binding = _binding) uniform
#define OC_UBO(_set, _binding, _blockname) layout (set = _set, binding = _binding) uniform _blockname

#if defined(__VERSION__) && __VERSION__ <= 120
    #if defined(SHADER_STAGE_FRAGMENT)
        vec4 texture(sampler1D s, float coord, float bias) { return texture1D(s, coord, bias); }
        vec4 texture(sampler2D s, vec2 coord,  float bias) { return texture2D(s, coord, bias); }
        vec4 texture(sampler3D s, vec3 coord,  float bias) { return texture3D(s, coord, bias); }
    #endif
#endif
