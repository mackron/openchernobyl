mkdir out

::Vulkan Shaders
glslangValidator -V ../../../source/oc/ocEngine/Graphics/Vulkan/Shaders/ocTest.glsl.vert -o out/ocTest.spv.vert
dred -f file2chex out/ocTest.spv.vert -n g_VertexShader_Test > ../../../source/oc/ocEngine/Graphics/Vulkan/Shaders/ocTest.spv.vert.h
glslangValidator -V ../../../source/oc/ocEngine/Graphics/Vulkan/Shaders/ocTest.glsl.frag -o out/ocTest.spv.frag
dred -f file2chex out/ocTest.spv.frag -n g_FragmentShader_Test > ../../../source/oc/ocEngine/Graphics/Vulkan/Shaders/ocTest.spv.frag.h

::OpenGL Shaders
glslangValidator ../../../source/oc/ocEngine/Graphics/OpenGL/Shaders/ocTest.glsl.vert
dred -f file2cstring ../../../source/oc/ocEngine/Graphics/OpenGL/Shaders/ocTest.glsl.vert -n g_VertexShader_Test > ../../../source/oc/ocEngine/Graphics/OpenGL/Shaders/ocTest.glsl.vert.h
glslangValidator ../../../source/oc/ocEngine/Graphics/OpenGL/Shaders/ocTest.glsl.frag
dred -f file2cstring ../../../source/oc/ocEngine/Graphics/OpenGL/Shaders/ocTest.glsl.frag -n g_FragmentShader_Test > ../../../source/oc/ocEngine/Graphics/OpenGL/Shaders/ocTest.glsl.frag.h
