// Copyright (C) 2017 David Reid. See included LICENSE file.

// Compiles all shaders.
//
// Returns an error if _any_ shader fails to compile.
ocResult ocBuildCompileShaders(int argc, char** argv);

// Retrieves the name of the variable that's used for the given file name.
//
// The name of the in-code variable is derived from the name of the shader's file.
ocResult ocBuildGetShaderVariableNameFromFileName(char* dst, size_t dstSize, const char* shaderFileName);