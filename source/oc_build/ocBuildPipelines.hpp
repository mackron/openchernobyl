// Copyright (C) 2018 David Reid. See included LICENSE file.

struct ocBuildPipelineDefinition
{
    struct json_value_s* pJSONRoot;
    const char* name;
    const char* inherits;
    const char* vertShader;
    const char* fragShader;
    const char* vertexFormat;
    const char* primitiveTopology;
    ocBool32 noMaterial;
    ocBool32 supportMSAA;
    ocBool32 _isFullyLoaded;    // Used to determine if the definition needs to be loaded.
};

// Compiles all pipelines.
//
// Returns an error if _any_ pipeline fails to compile.
ocResult ocBuildCompilePipelines(int argc, char** argv, ocBuildGraphicsContext* pGraphicsBuildContext);