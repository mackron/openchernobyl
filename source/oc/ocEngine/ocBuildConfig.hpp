// Copyright (C) 2018 David Reid. See included LICENSE file.

#define OC_STRINGIFY(x)         #x
#define OC_XSTRINGIFY(x)        OC_STRINGIFY(x)

///////////////////////////////////////////////////////////////////////////////
// Application Configuration

#ifndef OC_PRODUCT_NAME
#define OC_PRODUCT_NAME         "Open Chernobyl"
#endif

#ifndef OC_PRODUCT_DESCRIPTION
#define OC_PRODUCT_DESCRIPTION  "Open Chernobyl"
#endif

#ifndef OC_COMPANY_NAME
#define OC_COMPANY_NAME         "David Reid"
#endif

#ifndef OC_VERSION_MAJOR
#define OC_VERSION_MAJOR        0
#endif
#ifndef OC_VERSION_MINOR
#define OC_VERSION_MINOR        0
#endif
#ifndef OC_VERSION_REVISION
#define OC_VERSION_REVISION     1
#endif
#ifndef OC_VERSION_STRING
#define OC_VERSION_STRING       OC_XSTRINGIFY(OC_VERSION_MAJOR) "." OC_XSTRINGIFY(OC_VERSION_MINOR) "." OC_XSTRINGIFY(OC_VERSION_REVISION)
#endif

#ifndef OC_CONFIG_NAME
#define OC_CONFIG_NAME          "openchernobyl"
#endif

#ifndef OC_LOG_FILE_NAME
#define OC_LOG_FILE_NAME        OC_CONFIG_NAME".log"
#endif

///////////////////////////////////////////////////////////////////////////////


#ifdef _WIN32
#define OC_WIN32
#ifdef _WIN64
#define OC_64BIT
#else
#define OC_32BIT
#endif
#else
#define OC_POSIX
#endif

#ifdef __linux__
#define OC_LINUX
#define OC_X11
#endif

#ifdef __GNUC__
#ifdef __LP64__
#define OC_64BIT
#else
#define OC_32BIT
#endif
#endif

#if !defined(OC_64BIT) && !defined(OC_32BIT)
#include <stdint.h>
#if INTPTR_MAX == INT64_MAX
#define OC_64BIT
#else
#define OC_32BIT
#endif
#endif

#ifndef NDEBUG
#define OC_DEBUG
#else
#define OC_RELEASE
#endif


#ifndef OC_MAX_PATH
#define OC_MAX_PATH             4096
#endif


// Logging levels.
#define OC_LOG_LEVEL_NONE       0
#define OC_LOG_LEVEL_ERROR      1
#define OC_LOG_LEVEL_WARNING    2
#define OC_LOG_LEVEL_INFO       3
#define OC_LOG_LEVEL_VERBOSE    4
#define OC_LOG_LEVEL_ALL        (~0U)

#ifndef OC_LOG_LEVEL
#ifdef OC_DEBUG
#define OC_LOG_LEVEL            OC_LOG_LEVEL_ALL
#else
#define OC_LOG_LEVEL            OC_LOG_LEVEL_CRITICAL
#endif
#endif


// Currently, only a single GPU device is used for everything. In the future we may add support for multi-GPU configurations.
#ifndef OC_MAX_GPU_DEVICES
#define OC_MAX_GPU_DEVICES      1
#endif

#ifndef OC_MAX_GPU_THREADS
#define OC_MAX_GPU_THREADS      8
#endif

#if !defined OC_MAX_COMPONENTS
#define OC_MAX_COMPONENTS       8
#endif
#if OC_MAX_COMPONENTS > 65535
error "OC_MAX_COMPONENTS cannot exceed 65535."
#endif

#ifndef OC_MAX_RENDER_TARGETS
#define OC_MAX_RENDER_TARGETS   8
#endif


#if !defined OC_USE_OPENGL && !defined OC_USE_VULKAN && !defined OC_USE_CUSTOM_RENDERER
#define OC_USE_OPENGL
//#define OC_USE_VULKAN
#endif
