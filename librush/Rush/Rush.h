#pragma once

#include <stddef.h>

// try to detect platform, if necessary

#if !defined(RUSH_PLATFORM_WINDOWS) && !defined(RUSH_PLATFORM_LINUX) && !defined(RUSH_PLATFORM_MACOS) && !defined(RUSH_PLATFORM_IOS)
#   if defined(_WIN32)
#       define RUSH_PLATFORM_WINDOWS
#   endif
#   if defined(__linux__)
#       define RUSH_PLATFORM_LINUX
#   endif
#   if defined(__APPLE__)
#       if defined(__MACH__)
#           define RUSH_PLATFORM_MACOS
#       else
#           define RUSH_PLATFORM_IOS
#       endif
#   endif
#endif

namespace Rush
{
    /// @addtogroup base
    /// @{

    //////////////////////////////////////////////////////////////////////////
    // Types

    typedef unsigned char       uint8;
    typedef unsigned short      uint16;
    typedef unsigned int        uint32;
    typedef unsigned long long  uint64;

    typedef signed char         int8;
    typedef signed short        int16;
    typedef signed int          int32;
    typedef signed long long    int64;

    //////////////////////////////////////////////////////////////////////////
    // Common defines

    #if defined(_MSC_VER)

        #define INLINE    __forceinline
        #define NOINLINE __declspec(noinline)
        #define R_ALIGN(x) __declspec(align(x))
        #define R_BREAK {__debugbreak();}
        #define R_FUNCTION __FUNCTION__
    
    #else // _MSC_VER

        #define INLINE inline
        #define NOINLINE
        #define R_ALIGN(x) __attribute__((aligned(x)))
        #define R_BREAK __builtin_trap()
    
        #if defined(RUSH_PLATFORM_IOS)
            #define R_FUNCTION "<TODO: Function name here>"
		#elif defined(RUSH_PLATFORM_MACOS)
			#define R_FUNCTION __func__
		#elif defined(RUSH_PLATFORM_LINUX)
			#define R_FUNCTION __func__
		#elif defined(RUSH_PLATFORM_NACL)
			#define R_FUNCTION __func__
        #else
            #define R_FUNCTION "<TODO: Function name here>"
        #endif

    #endif // _MSC_VER

    #define RESTRICT __restrict

    #ifndef NULL 
        #define NULL 0
    #endif //NULL

    #define R_UNUSED( v ) { (void)v; }

    #define RUSH_DISALLOW_COPY_AND_ASSIGN(T) T(const T&); void operator=(const T&) 


    template <typename T> inline T rush_min(T i, T j) { return (i < j) ? i : j; }
    template <typename T> inline T rush_max(T i, T j) { return (i > j) ? i : j; }

    /// @}
}

#ifdef RUSH_EXTERN_PLATFORM
#include <RushExternPlatform.h>
#endif //RUSH_EXTERN_PLATFORM

#ifdef RUSH_USING_NAMESPACE
using namespace Rush;
#endif //RUSH_USING_NAMESPACE
