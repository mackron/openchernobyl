// Copyright (C) 2017 David Reid. See included LICENSE file.

typedef int ocResult;

// General
#define OC_RESULT_SUCCESS                        0
#define OC_RESULT_UNKNOWN_ERROR                 -1
#define OC_RESULT_INVALID_ARGS                  -2
#define OC_RESULT_INVALID_OPERATION             -3
#define OC_RESULT_OUT_OF_MEMORY                 -4
#define OC_RESULT_NO_BACKEND                    -5
#define OC_RESULT_PERMISSION_DENIED             -6
#define OC_RESULT_FEATURE_NOT_SUPPORTED         -7
#define OC_RESULT_TOO_MANY_COMPONENTS           -8

// Graphics
#define OC_RESULT_FAILED_TO_INIT_GRAPHICS       -1024
#define OC_RESULT_TOO_MANY_RENDER_TARGETS       -1025
#define OC_RESULT_INVALID_FRAMEBUFFER           -1026
#define OC_RESULT_SHADER_ERROR                  -1027

// Audio
#define OC_RESULT_FAILED_TO_INIT_AUDIO          -2048

// File System.
#define OC_RESULT_DOES_NOT_EXIST                -3072
#define OC_RESULT_ALREADY_EXISTS                -3073
#define OC_RESULT_TOO_MANY_OPEN_FILES           -3074
#define OC_RESULT_FILE_NOT_IN_WRITE_DIRECTORY   -3075
#define OC_RESULT_PATH_TOO_LONG                 -3076
#define OC_RESULT_NO_SPACE                      -3077
#define OC_RESULT_NOT_DIRECTORY                 -3078
#define OC_RESULT_TOO_LARGE                     -3079
#define OC_RESULT_AT_END_OF_FILE                -3080
#define OC_RESULT_INVALID_ARCHIVE               -3081
#define OC_RESULT_NEGATIVE_SEEK                 -3082
#define OC_RESULT_CORRUPT_FILE                  -3083

// Resources
#define OC_RESULT_FAILED_TO_LOAD_RESOURCE       -4096
#define OC_RESULT_UNKNOWN_RESOURCE_TYPE         -4097
#define OC_RESULT_UNSUPPORTED_RESOURCE_TYPE     -4098
