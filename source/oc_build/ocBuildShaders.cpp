// Copyright (C) 2018 David Reid. See included LICENSE file.

struct ocBuildShadersContext
{
    ocString pOutputFileData_OpenGL;
    ocString pOutputFileData_Vulkan;
};

ocResult ocBuildShadersContextInit(ocBuildShadersContext* pContext)
{
    if (pContext == NULL) {
        return OC_INVALID_ARGS;
    }

    ocZeroObject(pContext);
    return OC_SUCCESS;
}

ocResult ocBuildShadersContextUninit(ocBuildShadersContext* pContext)
{
    if (pContext == NULL) {
        return OC_INVALID_ARGS;
    }

    ocFreeString(pContext->pOutputFileData_OpenGL);
    ocFreeString(pContext->pOutputFileData_Vulkan);

    return OC_SUCCESS;
}


#ifdef OC_WIN32
#define OC_BUILD_MCPP_PATH     "\"build\\oc_build\\bin\\mcpp-2.7.2\\bin\\mcpp.exe\""
#define OC_BUILD_GLSLANG_PATH  "build\\oc_build\\bin\\glslangValidator.exe"
#else
#define OC_BUILD_MCPP_PATH     "\"build/oc_build/bin/mcpp-2.7.2/bin/mcpp\""
#define OC_BUILD_GLSLANG_PATH  "build/oc_build/bin/glslangValidator"
#endif

#define OC_BUILD_SHADERS_INDIR_OLD "source/oc/ocEngine/Graphics/_Pipelines/Shaders"

#define OC_BUILD_MCPP_CMD(infile, outdir, options, cmd) \
    snprintf(cmd, sizeof(cmd), "%s %s/%s -o %s/%s/%s -P -k %s", OC_BUILD_MCPP_PATH, OC_BUILD_SHADERS_INDIR_OLD, infile, OC_BUILD_INTERMEDIATE_DIRECTORY, outdir, infile, options)

#define OC_BUILD_GLSLANG_CMD_VULKAN(infile, outdir, options, cmd) \
    snprintf(cmd, sizeof(cmd), "%s %s \"%s/vulkan/%s\" -o \"%s/vulkan/%s.spv\"", OC_BUILD_GLSLANG_PATH, options, OC_BUILD_INTERMEDIATE_DIRECTORY, infile, OC_BUILD_INTERMEDIATE_DIRECTORY, infile)

#define OC_BUILD_GLSLANG_CMD_OPENGL(infile, outdir, options, cmd) \
    snprintf(cmd, sizeof(cmd), "%s %s \"%s/opengl/%s\"", OC_BUILD_GLSLANG_PATH, options, OC_BUILD_INTERMEDIATE_DIRECTORY, infile)

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
    snprintf(inputFilePath, sizeof(inputFilePath), "%s/%s", OC_BUILD_SHADERS_INDIR_OLD, filePath);

    char intermediateFilePath[OC_MAX_PATH];
    snprintf(intermediateFilePath, sizeof(intermediateFilePath), "%s%s", OC_BUILD_INTERMEDIATE_DIRECTORY "/opengl/", filePath);


    ocCreateDirectoryRecursive(OC_BUILD_INTERMEDIATE_DIRECTORY "/opengl");

    // Preprocess.
    char options[1024];
    snprintf(options, sizeof(options),
        "-DOPENGL -D__VERSION__=%s -DSHADER_STAGE_%s",
        "120",
        ocBuildGetShaderStageStringFromFilePath(filePath));

    OC_BUILD_MCPP_CMD(filePath, "opengl", options, cmd);
    systemResult = ocSystem(cmd);
    if (systemResult != 0) {
        return OC_FAILED_TO_COMPILE_SHADER;
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
        return OC_FAILED_TO_COMPILE_SHADER;
    }

    return OC_SUCCESS;
}

ocResult ocBuildCompileShader_Vulkan(const char* filePath)
{
    char cmd[4096];
    int systemResult;

    char inputFilePath[OC_MAX_PATH];
    snprintf(inputFilePath, sizeof(inputFilePath), "%s/%s", OC_BUILD_SHADERS_INDIR_OLD, filePath);

    char intermediateFilePath[OC_MAX_PATH];
    snprintf(intermediateFilePath, sizeof(intermediateFilePath), "%s%s", OC_BUILD_INTERMEDIATE_DIRECTORY "/vulkan/", filePath);


    ocCreateDirectoryRecursive(OC_BUILD_INTERMEDIATE_DIRECTORY "/vulkan");

    // Preprocess.
    char options[1024];
    snprintf(options, sizeof(options),
        "-DVULKAN -D__VERSION__=%s -DSHADER_STAGE_%s",
        "450",
        ocBuildGetShaderStageStringFromFilePath(filePath));

    OC_BUILD_MCPP_CMD(filePath, "vulkan", options, cmd);
    systemResult = ocSystem(cmd);
    if (systemResult != 0) {
        return OC_FAILED_TO_COMPILE_SHADER;
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
        return OC_FAILED_TO_COMPILE_SHADER;
    }

    return OC_SUCCESS;
}

ocResult ocBuildCompileShader(const char* filePath)
{
    ocResult result = OC_SUCCESS;

    printf("Compiling Shader: %s\n", filePath);

    result = ocBuildCompileShader_OpenGL(filePath);
    if (result != OC_SUCCESS) {
        printf("Failed to compile OpenGL shader %s.\n\n", filePath);
        return result;
    }

    result = ocBuildCompileShader_Vulkan(filePath);
    if (result != OC_SUCCESS) {
        printf("Failed to compile Vulkan shader %s.\n\n", filePath);
        return result;
    }

    return result;
}


ocResult ocBuildGenerateShaderCCode_OpenGL(ocBuildShadersContext* pContext, const char* filePath)
{
    char inputFilePath[OC_MAX_PATH];
    snprintf(inputFilePath, sizeof(inputFilePath), "%s/opengl/%s", OC_BUILD_INTERMEDIATE_DIRECTORY, filePath);

    void* pInputFileData;
    size_t inputFileSize;
    ocResult result = ocOpenAndReadFile(inputFilePath, &pInputFileData, &inputFileSize);
    if (result != OC_SUCCESS) {
        return result;
    }

    char variableName[256];
    ocBuildGetShaderVariableNameFromFileName(variableName, sizeof(variableName), filePath);

    // For now we output the file data as a C-style string.
    char* pOutputData = ocBuildBufferToCString((const unsigned char*)pInputFileData, inputFileSize, variableName);
    if (pOutputData == NULL) {
        return OC_ERROR;
    }

    pContext->pOutputFileData_OpenGL = ocAppendString(pContext->pOutputFileData_OpenGL, pOutputData);
    pContext->pOutputFileData_OpenGL = ocAppendString(pContext->pOutputFileData_OpenGL, "\n\n");

    ocFree(pOutputData);
    ocFree(pInputFileData);
    return OC_SUCCESS;
}

ocResult ocBuildGenerateShaderCCode_Vulkan(ocBuildShadersContext* pContext, const char* filePath)
{
    char inputFilePath[OC_MAX_PATH];
    snprintf(inputFilePath, sizeof(inputFilePath), "%s/vulkan/%s.spv", OC_BUILD_INTERMEDIATE_DIRECTORY, filePath);

    void* pInputFileData;
    size_t inputFileSize;
    ocResult result = ocOpenAndReadFile(inputFilePath, &pInputFileData, &inputFileSize);
    if (result != OC_SUCCESS) {
        return result;
    }

    char variableName[256];
    ocBuildGetShaderVariableNameFromFileName(variableName, sizeof(variableName), filePath);

    // For now we output the file data as a C-style string.
    char* pOutputData = ocBuildBufferToCArray((const unsigned char*)pInputFileData, inputFileSize, variableName);
    if (pOutputData == NULL) {
        return OC_ERROR;
    }

    pContext->pOutputFileData_Vulkan = ocAppendString(pContext->pOutputFileData_Vulkan, pOutputData);
    pContext->pOutputFileData_Vulkan = ocAppendString(pContext->pOutputFileData_Vulkan, "\n\n");

    ocFree(pOutputData);
    ocFree(pInputFileData);
    return OC_SUCCESS;
}

ocResult ocBuildGenerateShaderCCode(ocBuildShadersContext* pContext, const char* filePath)
{
    ocResult result = OC_SUCCESS;

    result = ocBuildGenerateShaderCCode_OpenGL(pContext, filePath);
    if (result != OC_SUCCESS) {
        printf("Failed to generate C code for OpenGL shader %s\n", filePath);
        return result;
    }

    result = ocBuildGenerateShaderCCode_Vulkan(pContext, filePath);
    if (result != OC_SUCCESS) {
        printf("Failed to generate C code for Vulkan shader %s\n", filePath);
        return result;
    }

    return result;
}

ocResult ocBuildCompileShaders(int argc, char** argv)
{
    (void)argc;
    (void)argv;

    ocBool32 hasError = OC_FALSE;
    ocResult result = OC_SUCCESS;

    ocBuildShadersContext context;
    result = ocBuildShadersContextInit(&context);
    if (result != OC_SUCCESS) {
        return result;
    }


    result = ocBuildCompileShader("ocShader_Default.vert");
    if (result != OC_SUCCESS) {
        hasError = OC_TRUE;
    }

    result = ocBuildCompileShader("ocShader_Default.frag");
    if (result != OC_SUCCESS) {
        hasError = OC_TRUE;
    }


    if (hasError) {
        return OC_ERROR;
    }


    // At this point, all shaders have been compiled and are sitting in the intermediary directory. We now need to iterate over each one and
    // generate the appropriate C code.
    result = ocBuildGenerateShaderCCode(&context, "ocShader_Default.vert");
    if (result != OC_SUCCESS) {
        return result;
    }

    result = ocBuildGenerateShaderCCode(&context, "ocShader_Default.frag");
    if (result != OC_SUCCESS) {
        return result;
    }


    // At this point we should have the contents of the auto-generated files in memory, so now we need to dump it to an actual file.
    result = ocBuildAppendStringToFile(OC_BUILD_INTERMEDIATE_DIRECTORY "/opengl/ocGraphics_OpenGL_Autogen.cpp", context.pOutputFileData_OpenGL);
    if (result != OC_SUCCESS) {
        return result;
    }

    result = ocBuildAppendStringToFile(OC_BUILD_INTERMEDIATE_DIRECTORY "/vulkan/ocGraphics_Vulkan_Autogen.cpp", context.pOutputFileData_Vulkan);
    if (result != OC_SUCCESS) {
        return result;
    }


    ocBuildShadersContextUninit(&context);
    return OC_SUCCESS;
}


ocResult ocBuildGetShaderVariableNameFromFileName(char* dst, size_t dstSize, const char* shaderFileName)
{
    if (dst == NULL || dstSize == 0 || shaderFileName == NULL) {
        return OC_INVALID_ARGS;
    }

    char variableNameBase[256];
    drpath_copy_and_remove_extension(variableNameBase, sizeof(variableNameBase), drpath_file_name(shaderFileName));
    snprintf(dst, dstSize, "g_%s_%s", variableNameBase, ocBuildGetShaderStageStringFromFilePath(shaderFileName));

    return OC_SUCCESS;
}