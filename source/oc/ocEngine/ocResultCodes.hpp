// Copyright (C) 2018 David Reid. See included LICENSE file.

typedef int ocResult;

// General
#define OC_SUCCESS                              0
#define OC_ERROR                                -1  // A generic error.
#define OC_INVALID_ARGS                         -2
#define OC_INVALID_OPERATION                    -3
#define OC_OUT_OF_MEMORY                        -4
#define OC_NO_BACKEND                           -5
#define OC_PERMISSION_DENIED                    -6
#define OC_FEATURE_NOT_SUPPORTED                -7
#define OC_TOO_MANY_COMPONENTS                  -8

// Graphics
#define OC_FAILED_TO_INIT_GRAPHICS              -1024
#define OC_TOO_MANY_RENDER_TARGETS              -1025
#define OC_INVALID_FRAMEBUFFER                  -1026
#define OC_SHADER_ERROR                         -1027
#define OC_FAILED_TO_COMPILE_SHADER             -1028

// Audio
#define OC_FAILED_TO_INIT_AUDIO                 -2048

// File System.
#define OC_DOES_NOT_EXIST                       -3072
#define OC_ALREADY_EXISTS                       -3073
#define OC_TOO_MANY_OPEN_FILES                  -3074
#define OC_FILE_NOT_IN_WRITE_DIRECTORY          -3075
#define OC_PATH_TOO_LONG                        -3076
#define OC_NO_SPACE                             -3077
#define OC_NOT_DIRECTORY                        -3078
#define OC_TOO_LARGE                            -3079
#define OC_AT_END_OF_FILE                       -3080
#define OC_INVALID_ARCHIVE                      -3081
#define OC_NEGATIVE_SEEK                        -3082
#define OC_CORRUPT_FILE                         -3083

// Resources
#define OC_FAILED_TO_LOAD_RESOURCE              -4096
#define OC_UNKNOWN_RESOURCE_TYPE                -4097
#define OC_UNSUPPORTED_RESOURCE_TYPE            -4098
