static const char* g_FragmentShader_Test = {
    "#version 120\n"
    "\r\n"
    "uniform sampler2D Texture;\n"
    "\n"
    "varying vec2 TexCoord;\n"
    "varying vec3 Normal;\r\n"
    "\r\n"
    "void main()\r\n"
    "{\r\n"
    "    //gl_FragColor = vec4(1, 1, 0, 1);\n"
    "    gl_FragColor = texture2D(Texture, TexCoord);\r\n"
    "}"
};