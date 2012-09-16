#pragma once

#include "Rush.h"

#define RUSH_RENDER_API_NONE    0
#define RUSH_RENDER_API_DX9     1
#define RUSH_RENDER_API_DX11    2
#define RUSH_RENDER_API_GLES2   3
#define RUSH_RENDER_API_GL      4
#define RUSH_RENDER_API_EXTERN  5

// Select API


#ifdef RUSH_DX9
#define RUSH_RENDER_API RUSH_RENDER_API_DX9
#endif //RUSH_DX9

#ifdef RUSH_DX11
#define RUSH_RENDER_API RUSH_RENDER_API_DX11
#endif //RUSH_DX11

#ifdef RUSH_GLES2
#define RUSH_RENDER_API RUSH_RENDER_API_GLES2
#endif //RUSH_GLES2

#ifdef RUSH_OPENGL
#define RUSH_RENDER_API RUSH_RENDER_API_GL
#endif //RUSH_OPENGL

#ifdef RUSH_EXTERN_GFX
#define RUSH_RENDER_API RUSH_RENDER_API_EXTERN
#endif //RUSH_EXTERN_GFX


// Override render API on some platforms

#ifdef RUSH_PLATFORM_NACL
#undef RUSH_RENDER_API
#define RUSH_RENDER_API RUSH_RENDER_API_GLES2
#endif //RUSH_PLATFORM_NACL

#ifdef RUSH_PLATFORM_IOS
#undef RUSH_RENDER_API
#define RUSH_RENDER_API RUSH_RENDER_API_GLES2
#endif //RUSH_PLATFORM_IOS

#ifdef RUSH_PLATFORM_MACOS
#undef RUSH_RENDER_API
#define RUSH_RENDER_API RUSH_RENDER_API_GL
#endif //RUSH_PLATFORM_MACOS

#ifdef RUSH_PLATFORM_LINUX
#undef RUSH_RENDER_API
#define RUSH_RENDER_API RUSH_RENDER_API_GL
#endif //RUSH_PLATFORM_LINUX

// If we got here and did not select an API, select some reasonable default


#ifndef RUSH_RENDER_API
#define RUSH_RENDER_API RUSH_RENDER_API_NONE
#endif //RUSH_RENDER_API
