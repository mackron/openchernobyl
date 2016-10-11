// Copyright (C) 2016 David Reid. See included LICENSE file.

#include "ocEngine.hpp"

#include "ocMisc.cpp"
#include "ocImageUtils.cpp"
#include "ocCommandLine.cpp"
#include "ocPlatformLayer.cpp"
#include "ocMath.cpp"
#include "ocCamera.cpp"
#include "ocThreading.cpp"
#include "ocFileSystem.cpp"
#include "ocLogger.cpp"
#include "Graphics/ocGraphics.cpp"
#include "Audio/ocAudio.cpp"
#include "Physics/ocPhysics.cpp"
#include "Components/ocComponents.cpp"
#include "ocResourceLoader.cpp"
#include "ocResourceLibrary.cpp"
#include "ocWorldObject.cpp"
#include "ocWorld.cpp"
#include "ocEngineContext.cpp"



// External libraries.
#if defined(_MSC_VER)
    #pragma warning(push)
    #pragma warning(disable:4244)
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

#define STB_IMAGE_WRITE_IMPLEMENTATION
#define STBI_WRITE_NO_STDIO
#include "../../external/stb/stb_image_write.h"

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

#define DR_IMPLEMENTATION
#define DR_FLAC_IMPLEMENTATION
#define DR_WAV_IMPLEMENTATION
#define DR_AUDIO_IMPLEMENTATION
#define DR_FS_IMPLEMENTATION
#define DR_OBJ_IMPLEMENTATION
#ifdef OC_USE_EXTERNAL_DR_LIBS
    #include "../../../../../dr_libs/dr.h"
    #include "../../../../../dr_libs/dr_flac.h"
    #include "../../../../../dr_libs/dr_wav.h"
    #include "../../../../../dr_libs/dr_audio.h"
    #include "../../../../../dr_libs/dr_fs.h"
    #include "../../../../../dr_libs/dr_obj.h"
#else
    #include "../../external/dr_libs/dr.h"
    #include "../../external/dr_libs/dr_flac.h"
    #include "../../external/dr_libs/dr_wav.h"
    #include "../../external/dr_libs/dr_audio.h"
    #include "../../external/dr_libs/dr_fs.h"
    #include "../../external/dr_libs/dr_obj.h"
#endif

#ifdef OC_USE_OPENGL
    #define DR_GL_IMPLEMENTATION
    #ifdef OC_USE_EXTERNAL_DR_LIBS
        #include "../../../../dr_graphics/dr_gl.h"
    #else
        #include "../../external/dr_graphics/dr_gl.h"
    #endif
#endif

#ifdef OC_USE_VULKAN
    #define DR_VULKAN_IMPLEMENTATION
    #ifdef OC_USE_EXTERNAL_DR_LIBS
        #include "../../../../dr_graphics/dr_vulkan.h"
    #else
        #include "../../external/dr_graphics/dr_vulkan.h"
    #endif
#endif


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
