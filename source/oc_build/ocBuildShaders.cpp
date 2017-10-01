// Copyright (C) 2017 David Reid. See included LICENSE file.

#ifdef OC_WIN32
#define OC_BUILD_MCPP_PATH     "\"build\\oc_build\\bin\\mcpp-2.7.2\\bin\\mcpp.exe\""
#define OC_BUILD_GLSLANG_PATH  "build\\oc_build\\bin\\glslangValidator.exe"
#else
#define OC_BUILD_MCPP_PATH     "\"build/oc_build/bin/mcpp-2.7.2/bin/mcpp\""
#define OC_BUILD_GLSLANG_PATH  "build/oc_build/bin/glslangValidator"
#endif

#define OC_BUILD_SHADERS_INDIR     "source/oc/ocEngine/Graphics/_Pipelines/Shaders/"
#define OC_BUILD_SHADERS_INTERMEDIATEDIR    "build/oc_build/bin/out/"

#define OC_BUILD_MCPP_CMD(infile, outdir, options, cmd) \
    snprintf(cmd, sizeof(cmd), "%s %s%s -o %s%s/%s -P -k %s", OC_BUILD_MCPP_PATH, OC_BUILD_SHADERS_INDIR, infile, OC_BUILD_SHADERS_INTERMEDIATEDIR, outdir, infile, options)

#define OC_BUILD_GLSLANG_CMD_VULKAN(infile, outdir, options, cmd) \
    snprintf(cmd, sizeof(cmd), "%s %s \"%svulkan/%s\" -o %s%s", OC_BUILD_GLSLANG_PATH, options, OC_BUILD_SHADERS_INTERMEDIATEDIR, infile, OC_BUILD_SHADERS_INTERMEDIATEDIR, infile)

#define OC_BUILD_GLSLANG_CMD_OPENGL(infile, outdir, options, cmd) \
    snprintf(cmd, sizeof(cmd), "%s %s \"%sopengl/%s\"", OC_BUILD_GLSLANG_PATH, options, OC_BUILD_SHADERS_INTERMEDIATEDIR, infile)

const char* ocBuildGetShaderStageStringFromFilePath(const char* filePath)
{
    if (drpath_extension_equal(filePath, "vert")) {
        return "VERTEX";
    }
    if (drpath_extension_equal(filePath, "tesc")) {
        return "TESSELLATION_CONTROL";
    }
    if (drpath_extension_equal(filePath, "tese")) {
        return "TESSELLATION_EVALUATION";
    }
    if (drpath_extension_equal(filePath, "geom")) {
        return "GEOMETRY";
    }
    if (drpath_extension_equal(filePath, "frag")) {
        return "FRAGMENT";
    }
    if (drpath_extension_equal(filePath, "comp")) {
        return "COMPUTE";
    }

    return "GENERIC";
}

ocResult ocBuildCompileShader_OpenGL(const char* filePath)
{
    char cmd[4096];
    int systemResult;

    char inputFilePath[OC_MAX_PATH];
    snprintf(inputFilePath, sizeof(inputFilePath), "%s%s", OC_BUILD_SHADERS_INDIR, filePath);

    char intermediateFilePath[OC_MAX_PATH];
    snprintf(intermediateFilePath, sizeof(intermediateFilePath), "%s%s", OC_BUILD_SHADERS_INTERMEDIATEDIR "opengl/", filePath);


    ocCreateDirectoryRecursive(OC_BUILD_SHADERS_INTERMEDIATEDIR "opengl");

    // Preprocess.
    char options[1024];
    snprintf(options, sizeof(options),
        "-DOPENGL -D__VERSION__=%s -DSHADER_STAGE_%s",
        "120",
        ocBuildGetShaderStageStringFromFilePath(filePath));

    OC_BUILD_MCPP_CMD(filePath, "opengl", options, cmd);
    systemResult = ocSystem(cmd);
    if (systemResult != 0) {
        return OC_RESULT_FAILED_TO_COMPILE_SHADER;
    }

    // Compile.
    snprintf(options, sizeof(options), "");

    char prefix[1024];
    snprintf(prefix, sizeof(prefix),
        "#version 120\n"
        "\n");

    ocBuildPrependStringToFile(intermediateFilePath, prefix);
    OC_BUILD_GLSLANG_CMD_OPENGL(filePath, "opengl", "", cmd);
    systemResult = ocSystem(cmd);
    if (systemResult != 0) {
        return OC_RESULT_FAILED_TO_COMPILE_SHADER;
    }

    return OC_RESULT_SUCCESS;
}

ocResult ocBuildCompileShader_Vulkan(const char* filePath)
{
    char cmd[4096];
    int systemResult;

    char inputFilePath[OC_MAX_PATH];
    snprintf(inputFilePath, sizeof(inputFilePath), "%s%s", OC_BUILD_SHADERS_INDIR, filePath);

    char intermediateFilePath[OC_MAX_PATH];
    snprintf(intermediateFilePath, sizeof(intermediateFilePath), "%s%s", OC_BUILD_SHADERS_INTERMEDIATEDIR "vulkan/", filePath);


    ocCreateDirectoryRecursive(OC_BUILD_SHADERS_INTERMEDIATEDIR "vulkan");

    // Preprocess.
    char options[1024];
    snprintf(options, sizeof(options),
        "-DVULKAN -D__VERSION__=%s -DSHADER_STAGE_%s",
        "450",
        ocBuildGetShaderStageStringFromFilePath(filePath));

    OC_BUILD_MCPP_CMD(filePath, "vulkan", options, cmd);
    systemResult = ocSystem(cmd);
    if (systemResult != 0) {
        return OC_RESULT_FAILED_TO_COMPILE_SHADER;
    }

    // Compile.
    snprintf(options, sizeof(options), "-V");

    char prefix[1024];
    snprintf(prefix, sizeof(prefix),
        "#version 450\n"
        "\n");

    ocBuildPrependStringToFile(intermediateFilePath, prefix);
    OC_BUILD_GLSLANG_CMD_VULKAN(filePath, "vulkan", options, cmd);
    systemResult = ocSystem(cmd);
    if (systemResult != 0) {
        return OC_RESULT_FAILED_TO_COMPILE_SHADER;
    }

    return OC_RESULT_SUCCESS;
}

ocResult ocBuildCompileShader(const char* filePath)
{
    ocResult result = OC_RESULT_SUCCESS;

    printf("%s\n", filePath);

    result = ocBuildCompileShader_OpenGL(filePath);
    if (result != OC_RESULT_SUCCESS) {
        printf("Failed to compile OpenGL shader %s.\n\n", filePath);
        return result;
    }

    result = ocBuildCompileShader_Vulkan(filePath);
    if (result != OC_RESULT_SUCCESS) {
        printf("Failed to compile Vulkan shader %s.\n\n", filePath);
        return result;
    }

    return result;
}

ocResult ocBuildCompileShaders(int argc, char** argv)
{
    (void)argc;
    (void)argv;

    ocBool32 hasError = OC_FALSE;
    ocResult result = OC_RESULT_SUCCESS;

    result = ocBuildCompileShader("ocShader_Default.frag");
    if (result != OC_RESULT_SUCCESS) {
        hasError = OC_TRUE;
    }

    result = ocBuildCompileShader("ocShader_Default.vert");
    if (result != OC_RESULT_SUCCESS) {
        hasError = OC_TRUE;
    }


    if (hasError) {
        return OC_RESULT_UNKNOWN_ERROR;
    }

    return OC_RESULT_SUCCESS;
}