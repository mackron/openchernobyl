//# <<FRAG_Material_Properties>>

vec4 Material_Diffuse()
{
    return texture(FRAG_Material_DiffuseMap, FRAG_TexCoord, 0.0f) * FRAG_Material.DiffuseColor;
}

vec3 Material_Normal()
{
    return vec3(0, 0, 1);
}