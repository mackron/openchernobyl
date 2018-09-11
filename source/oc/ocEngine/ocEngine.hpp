// Copyright (C) 2018 David Reid. See included LICENSE file.

// To Compile:
//
// Include Paths:
//   - source/external/bullet3/src
//   - <path to Vulkan SDK>/Include
//
//
// To Link:
//   - Bullet:
//     - BulletDynamics  / BulletDynamics_Debug
//     - BulletCollision / BulletCollision_Debug
//     - LinearMath      / LinearMath_Debug

#ifndef OC_ENGINE_HPP
#define OC_ENGINE_HPP

#define OC_STRINGIFY(x)         #x
#define OC_XSTRINGIFY(x)        OC_STRINGIFY(x)

// Put the build config at the top in order to gain access to platform detection.
#include "ocBuildConfig.hpp"


// Standard headers.
#include <stdlib.h>
#include <vector>
#include <algorithm>


// Platform headers.
#ifdef OC_WIN32
#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <intrin.h>
#endif

#ifdef OC_X11
#include <X11/Xatom.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include <pthread.h>
#include <semaphore.h>
#endif

// External libraries.
#define STBI_WRITE_NO_STDIO
#define STB_VORBIS_HEADER_ONLY
#define STB_VORBIS_NO_STDIO
#include "../../external/stb/stb_image.h"
#include "../../external/stb/stb_image_write.h"
#include "../../external/stb/stb_vorbis.c"

//#include "../../external/stb/stretchy_buffer.h"   // <-- Doesn't work with C++ at the moment.

#ifndef DR_IMPLEMENTATION
    #include "../../external/dr_libs/old/dr.h"  // TODO: Remove this dependency
#endif

#include "../../external/dr_libs/dr_flac.h"
#include "../../external/dr_libs/dr_wav.h"
#include "../../external/dr_libs/old/dr_fs.h"   // TODO: Replace with dr-soft/dr_fs/dr_fs.h
#include "../../external/dr_libs/dr_obj.h"

#ifdef OC_ENABLE_PCX
#include "../../external/dr_libs/dr_pcx.h"
#endif


#ifdef OC_USE_OPENGL
    //#define DR_GL_VERSION 300
    #define DR_GL_VERSION 210
    #define DR_GL_ENABLE_EXT_framebuffer_blit
    #define DR_GL_ENABLE_EXT_framebuffer_multisample
    #define DR_GL_ENABLE_EXT_framebuffer_object
    #define DR_GL_ENABLE_ARB_texture_multisample
    #define DR_GL_ENABLE_EXT_swap_control
    #ifdef OC_DEBUG
        #define DR_GL_ENABLE_ARB_debug_output
    #endif
    #define DR_GL_NO_DEFAULT_INCLUDES
    #ifdef _WIN32
    #include <GL/gl.h>
    #include "../../external/GL/glext.h"
    #include "../../external/GL/wglext.h"
    #else
    #include <GL/glx.h>
    #include "../../external/GL/glxext.h"
    #include "../../external/GL/glext.h"
    #endif
    #include "../../external/dr_graphics/dr_gl.h"
#endif

#ifdef OC_USE_VULKAN
    #ifdef OC_WIN32
        #define VK_USE_PLATFORM_WIN32_KHR
    #endif
    #ifdef OC_X11
        #define VK_USE_PLATFORM_XLIB_KHR
    #endif
    #include "../../external/dr_graphics/dr_vulkan.h"
#endif

#include "../../external/mini_al/mini_al.h"

#if defined(_MSC_VER)
    #pragma warning(push)
    #pragma warning(disable:4127)
    #pragma warning(disable:4201)
    #pragma warning(disable:4305)
#endif
#include "../../external/bullet3/src/btBulletDynamicsCommon.h"
#if defined(_MSC_VER)
    #pragma warning(pop)
#endif

#if defined(_MSC_VER)
    #pragma warning(push)
    #pragma warning(disable:4201)
    #pragma warning(disable:4310)
    #pragma warning(disable:4324)
#endif
#include "../../external/glm/glm/glm.hpp"
#include "../../external/glm/glm/ext.hpp"
#if defined(_MSC_VER)
    #pragma warning(pop)
#endif

// Sized types.
#if defined(_MSC_VER) && _MSC_VER < 1600
typedef   signed char    ocInt8;
typedef unsigned char    ocUInt8;
typedef   signed short   ocInt16;
typedef unsigned short   ocUInt16;
typedef   signed int     ocInt32;
typedef unsigned int     ocUInt32;
typedef   signed __int64 ocInt64;
typedef unsigned __int64 ocUInt64;
#else
#include <stdint.h>
typedef int8_t           ocInt8;
typedef uint8_t          ocUInt8;
typedef int16_t          ocInt16;
typedef uint16_t         ocUInt16;
typedef int32_t          ocInt32;
typedef uint32_t         ocUInt32;
typedef int64_t          ocInt64;
typedef uint64_t         ocUInt64;
#endif
typedef float            ocFloat32;
typedef double           ocFloat64;
typedef size_t           ocSizeT;
typedef ocUInt8          ocBool8;
typedef ocUInt32         ocBool32;
#define OC_TRUE          1
#define OC_FALSE         0

#define OC_PRIVATE static

#if defined(_MSC_VER)
#define OC_INLINE static __inline
#else
#define OC_INLINE static inline
#endif

#define ocAssert(expression)        assert(expression)
#define ocCopyMemory(dst, src, sz)  memcpy(dst, src, sz)
#define ocZeroMemory(p, sz)         memset((p), 0, (sz))
#define ocZeroObject(p)             ocZeroMemory((p), sizeof(*(p)))
#define ocMalloc(sz)                malloc(sz)
#define ocCalloc(c, sz)             calloc(c, sz)
#define ocRealloc(p, sz)            realloc(p, sz)
#define ocMallocObject(type)        ((type*)ocMalloc(sizeof(type)))
#define ocCallocObject(type)        ((type*)ocCalloc(1, sizeof(type)))
#define ocFree(p)                   free(p)
#define ocCountOf(obj)              (sizeof(obj) / sizeof(obj[0]))
#define ocIsBitSet(set, bit)        (((set) & (bit)) != 0)
#define ocAlign(x, a)               ((((x) + (a) - 1) / (a)) * (a))

// ocOffsetPtr()
template <typename T, typename U>
T* ocOffsetPtr(T* p, U offset)
{
    return reinterpret_cast<T*>(reinterpret_cast<char*>(p) + offset);
}

template <typename T, typename U>
const T* ocOffsetPtr(const T* p, U offset)
{
    return reinterpret_cast<const T*>(reinterpret_cast<const char*>(p) + offset);
}

struct ocEngineContext;
struct ocGraphicsContext;
struct ocGraphicsDevice;
struct ocGraphicsWorld;
struct ocWorld;
struct ocWorldObject;

// Open Chernobyl headers.
#include "ocResultCodes.hpp"
#include "ocMisc.hpp"
#include "ocContainers.hpp"
#include "ocString.hpp"
#include "ocImageUtils.hpp"
#include "ocCommandLine.hpp"
#include "ocPlatformLayer.hpp"
#include "ocColor.hpp"
#include "ocMath.hpp"
#include "ocCamera.hpp"
#include "ocThreading.hpp"
#include "ocFileSystem.hpp"
#include "ocStreamReader.hpp"
#include "ocStreamWriter.hpp"
#include "ocLogger.hpp"
#include "Graphics/ocGraphics.hpp"
#include "Audio/ocAudio.hpp"
#include "Input/ocInput.hpp"
#include "Physics/ocPhysics.hpp"
#include "Components/ocComponents.hpp"
#include "ocOCD.hpp"
#include "ocResourceLoader.hpp"
#include "ocResourceLibrary.hpp"
#include "ocWorldObject.hpp"
#include "ocWorld.hpp"
#include "ocEngineContext.hpp"
#include "ocStretchyBuffer.hpp"     // <-- stb_stretchy_buffer with a C++ fix. Remove this once the upstream version is fixed.

#endif