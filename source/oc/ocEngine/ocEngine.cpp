// Copyright (C) 2018 David Reid. See included LICENSE file.

#if !defined(_WIN32)
#ifndef _LARGEFILE64_SOURCE
#define _LARGEFILE64_SOURCE
#endif
#ifndef _FILE_OFFSET_BITS
#define _FILE_OFFSET_BITS 64
#endif
#endif

#include "ocEngine.hpp"

#include "ocMisc.cpp"
#include "ocString.cpp"
#include "ocImageUtils.cpp"
#include "ocCommandLine.cpp"
#include "ocPlatformLayer.cpp"
#include "ocMath.cpp"
#include "ocCamera.cpp"
#include "ocThreading.cpp"
#include "ocFileSystem.cpp"
#include "ocStreamReader.cpp"
#include "ocStreamWriter.cpp"
#include "ocLogger.cpp"
#include "Graphics/ocGraphics.cpp"
#include "Audio/ocAudio.cpp"
#include "Input/ocInput.cpp"
#include "Physics/ocPhysics.cpp"
#include "Components/ocComponents.cpp"
#include "ocOCD.cpp"
#include "ocResourceLoader.cpp"
#include "ocResourceLibrary.cpp"
#include "ocWorldObject.cpp"
#include "ocWorld.cpp"
#include "ocEngineContext.cpp"



// External libraries.
#if defined(_MSC_VER)
    #pragma warning(push)
    #pragma warning(disable:4244)
    #pragma warning(disable:4100)
    #pragma warning(disable:4456)
    #pragma warning(disable:4505)
#endif
#if defined(__GNUC__)
    #pragma GCC diagnostic push
#if __GNUC__ >= 6
    #pragma GCC diagnostic ignored "-Wshift-negative-value"
#ifndef __clang__
    #pragma GCC diagnostic ignored "-Wmisleading-indentation"
#endif
#endif
#endif
#define STB_IMAGE_IMPLEMENTATION
#define STBI_NO_BMP
#define STBI_NO_PSD
#define STBI_NO_GIF
#define STBI_NO_HDR
#define STBI_NO_PIC
#define STBI_NO_PNM
#include "../../external/stb/stb_image.h"
#if defined(_MSC_VER)
    #pragma warning(pop)
#endif
#if defined(__GNUC__)
    #pragma GCC diagnostic pop
#endif

#if defined(_MSC_VER)
    #pragma warning(push)
    #pragma warning(disable:4996)
#endif
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define STBI_WRITE_NO_STDIO
#include "../../external/stb/stb_image_write.h"
#if defined(_MSC_VER)
    #pragma warning(pop)
#endif

#if defined(_MSC_VER)
    #pragma warning(push)
    #pragma warning(disable:4244)
    #pragma warning(disable:4100)
#endif
#define STB_DXT_IMPLEMENTATION
#include "../../external/stb/stb_dxt.h"
#if defined(_MSC_VER)
    #pragma warning(pop)
#endif

#define STB_PERLIN_IMPLEMENTATION
#include "../../external/stb/stb_perlin.h"

#ifndef DR_IMPLEMENTATION
    #define DR_IMPLEMENTATION
    #include "../../external/dr_libs/old/dr.h"  // TODO: Remove this dependency.
#endif

#define DR_FLAC_IMPLEMENTATION
#define DR_WAV_IMPLEMENTATION
#define DR_FS_IMPLEMENTATION
#define DR_OBJ_IMPLEMENTATION
#include "../../external/dr_libs/dr_flac.h"
#include "../../external/dr_libs/dr_wav.h"
#include "../../external/dr_libs/old/dr_fs.h"   // TODO: Replace this with dr-soft/dr_fs/dr_fs.h
#include "../../external/dr_libs/dr_obj.h"

#ifdef OC_ENABLE_PCX
#define DR_PCX_IMPLEMENTATION
#include "../../external/dr_libs/dr_pcx.h"
#endif

#ifdef OC_USE_OPENGL
    #define DR_GL_IMPLEMENTATION
    #include "../../external/dr_graphics/dr_gl.h"
#endif

#ifdef OC_USE_VULKAN
    #define DR_VULKAN_IMPLEMENTATION
    #include "../../external/dr_graphics/dr_vulkan.h"
#endif

#define MAL_IMPLEMENTATION
#include "../../external/mini_al/mini_al.h"


// Always define stb_vorbis last because it causes compilation errors otherwise.
#if defined(_MSC_VER)
    #pragma warning(push)
    #pragma warning(disable:4456)
    #pragma warning(disable:4457)
    #pragma warning(disable:4100)
    #pragma warning(disable:4244)
    #pragma warning(disable:4701)
    #pragma warning(disable:4245)
#endif
#if defined(__GNUC__)
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wunused-value"
    #pragma GCC diagnostic ignored "-Wunused-parameter"
    #ifndef __clang__
    #pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
    #endif
#endif
#undef STB_VORBIS_HEADER_ONLY
#include "../../external/stb/stb_vorbis.c"
#if defined(_MSC_VER)
    #pragma warning(pop)
#endif
#if defined(__GNUC__)
    #pragma GCC diagnostic pop
#endif
