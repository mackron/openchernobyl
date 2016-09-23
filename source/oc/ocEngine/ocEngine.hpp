// Copyright (C) 2016 David Reid. See included LICENSE file.

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
//   - Vulkan
//     - vulkan-1.lib

#ifndef OC_ENGINE_HPP
#define OC_ENGINE_HPP

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

#ifdef OC_WIN32
#define VK_USE_PLATFORM_WIN32_KHR
#endif
#ifdef OC_X11
#define VK_USE_PLATFORM_XLIB_KHR
#endif
//#include <vulkan/vulkan.h>


// External libraries.
#define STBI_WRITE_NO_STDIO
#define STB_VORBIS_HEADER_ONLY
#define STB_VORBIS_NO_STDIO
#include "../../external/stb/stb_image.h"
#include "../../external/stb/stb_image_write.h"
#include "../../external/stb/stb_dxt.h"
#include "../../external/stb/stb_vorbis.c"

//#include "../../external/stb/stretchy_buffer.h"   // <-- Doesn't work with C++ at the moment.

#ifdef OC_USE_EXTERNAL_DR_LIBS
#include "../../../../../../dr_libs/dr.h"
#include "../../../../../../dr_libs/dr_flac.h"
#include "../../../../../../dr_libs/dr_wav.h"
#include "../../../../../../dr_libs/dr_audio.h"
#include "../../../../../../dr_libs/dr_fs.h"
#include "../../../../../../dr_libs/dr_obj.h"
#else
#include "../../external/dr_libs/dr.h"
#include "../../external/dr_libs/dr_flac.h"
#include "../../external/dr_libs/dr_wav.h"
#include "../../external/dr_libs/dr_audio.h"
#include "../../external/dr_libs/dr_fs.h"
#include "../../external/dr_libs/dr_obj.h"
#endif

#ifdef OC_USE_OPENGL
#define DR_GL_VERSION 300
//#define DR_GL_VERSION 210
#define DR_GL_ENABLE_EXT_framebuffer_blit
#define DR_GL_ENABLE_EXT_framebuffer_multisample
#define DR_GL_ENABLE_EXT_framebuffer_object
#define DR_GL_ENABLE_ARB_texture_multisample
#define DR_GL_ENABLE_EXT_swap_control
#ifdef OC_USE_EXTERNAL_DR_LIBS
#include "../../../../../dr_graphics/dr_gl.h"
#else
#include "../../external/dr_graphics/dr_gl.h"
#endif
#endif

#ifdef OC_USE_VULKAN
#ifdef OC_USE_EXTERNAL_DR_LIBS
#include "../../../../../dr_graphics/dr_vulkan.h"
#else
#include "../../../../../dr_graphics/dr_vulkan.h"
#endif
#endif

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

#define OC_PRIVATE static

#define ocAssert(expression)    assert(expression)
#define ocZeroMemory(p, sz)     memset((p), 0, (sz))
#define ocZeroObject(p)         ocZeroMemory((p), sizeof(*(p)))
#define ocMalloc(sz)            malloc(sz)
#define ocMallocObject(type)    ((type*)malloc(sizeof(type)))
#define ocCallocObject(type)    ((type*)calloc(1, sizeof(type)))
#define ocFree(p)               free(p)
#define ocCountOf(obj)          (sizeof(obj) / sizeof(obj[0]))
#define ocIsBitSet(set, bit)    (((set) & (bit)) != 0)
#define ocAlign(x, a)           ((((x) + (a) - 1) / (a)) * (a));

// ocOffsetPtr()
template <typename T>
T* ocOffsetPtr(T* p, size_t offset)
{
    return reinterpret_cast<T*>(reinterpret_cast<char*>(p) + offset);
}

template <typename T>
const T* ocOffsetPtr(const T* p, size_t offset)
{
    return reinterpret_cast<const T*>(reinterpret_cast<const char*>(p) + offset);
}


typedef uint32_t ocBool32;

struct ocEngineContext;
struct ocGraphicsContext;
struct ocGraphicsDevice;
struct ocGraphicsWorld;
struct ocWorld;
struct ocWorldObject;

// Open Chernobyl headers.
#include "ocStretchyBuffer.hpp"     // <-- stb_stretchy_buffer with a C++ fix. Remove this once the upstream version is fixed.
#include "ocResultCodes.hpp"
#include "ocMisc.hpp"
#include "ocCommandLine.hpp"
#include "ocPlatformLayer.hpp"
#include "ocColor.hpp"
#include "ocMath.hpp"
#include "ocThreading.hpp"
#include "ocFileSystem.hpp"
#include "ocLogger.hpp"
#include "Graphics/ocGraphics.hpp"
#include "Audio/ocAudio.hpp"
#include "Physics/ocPhysics.hpp"
#include "Components/ocComponents.hpp"
#include "ocWorldObject.hpp"
#include "ocWorld.hpp"
#include "ocEngineContext.hpp"

#ifdef __cplusplus
template<class T>
static T * stb__sbgrowf(T * arr, int increment, int itemsize) {
    return (T*)stb__raw_sbgrowf((void *)arr, increment, itemsize);
}
#else
#define stb__sbgrowf stb__raw_sbgrowf
#endif

#endif