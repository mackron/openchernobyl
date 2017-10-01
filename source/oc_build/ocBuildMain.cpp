// Copyright (C) 2017 David Reid. See included LICENSE file.

#include "../oc/ocEngine/ocEngine.cpp"

#include "ocBuildUtils.hpp"
#include "ocBuildShaders.hpp"
#include "ocBuildPipelines.hpp"

#include "ocBuildUtils.cpp"
#include "ocBuildShaders.cpp"
#include "ocBuildPipelines.cpp"

int main(int argc, char** argv)
{
    // Set the root directory from the start so that everything going forward can assume it's set correctly. The root directory
    // should be set to the root directory of the source code repository.
    const char* pRootDir = ocCmdLineGetValue(argc, argv, "rootdir");
    if (pRootDir != NULL) {
        ocSetCurrentDirectory(pRootDir);
    }

    ocResult result = OC_RESULT_SUCCESS;

    // Compile shaders first.
    result = ocBuildCompileShaders(argc, argv);
    if (result != OC_RESULT_SUCCESS) {
        return -1;
    }

    // Pipelines come after shaders since they depend on them.
    result = ocBuildCompilePipelines(argc, argv);
    if (result != OC_RESULT_SUCCESS) {
        return -2;
    }

    return 0;
}