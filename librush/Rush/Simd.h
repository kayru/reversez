#pragma once

#if defined(_M_AMD64) || defined(_M_IX86) || __SSE2__

	#include <xmmintrin.h>
	#define RUSH_USE_INTRINSICS_SSE

#elif defined(XBOX360)

	#define RUSH_USE_INTRINSICS_VMX

#elif defined(__ARM_NEON__)

    #include <arm_neon.h>
	#define RUSH_USE_INTRINSICS_NEON

#else

	#include <math.h>
	#define RUSH_USE_SIMD_EMULATION

#endif

#include "Rush.h"

namespace Rush
{
	/// @addtogroup simd
	/// @{

#if defined(RUSH_USE_INTRINSICS_SSE)
	typedef __m128 simd4f;
	typedef simd4f& simd4f_param;
#elif defined(RUSH_USE_INTRINSICS_VMX)
	typedef __vector4 simd4f;
	typedef simd4f simd4f_param;
#elif defined(RUSH_USE_INTRINSICS_NEON)
    typedef float32x4_t simd4f;
	typedef float32x4_t simd4f_param;
#else	
	struct simd4f
	{
		float f[4];
	};
	typedef simd4f& simd4f_param;
#endif //RUSH_USE_INTRINSICS
    

	INLINE simd4f		simd4f_set(float x);
	INLINE simd4f		simd4f_load(const float* arr);
	INLINE void			simd4f_store(float* arr, simd4f v);
	INLINE void			simd4f_store_streaming(float* arr, simd4f v);

	INLINE simd4f		simd4f_zero();
	INLINE simd4f		simd4f_min(simd4f a, simd4f b);					// min(a,b)
	INLINE simd4f		simd4f_max(simd4f a, simd4f b);					// max(a,b)
	INLINE simd4f		simd4f_add(simd4f a, simd4f b);					// a+b
	INLINE simd4f		simd4f_sub(simd4f a, simd4f b);					// a-b
	INLINE simd4f		simd4f_mul(simd4f a, simd4f b);					// a*b
	INLINE simd4f		simd4f_madd(simd4f a, simd4f b, simd4f c);		// a*b+c
	INLINE simd4f		simd4f_div(simd4f a, simd4f b);					// a/b
	INLINE simd4f		simd4f_rsqrt(simd4f a);
	INLINE simd4f		simd4f_sqrt(simd4f a);
	INLINE simd4f		simd4f_rcp(simd4f a);

	INLINE simd4f		simd4f_splat_x(simd4f xyzw);
	INLINE simd4f		simd4f_splat_y(simd4f xyzw);
	INLINE simd4f		simd4f_splat_z(simd4f xyzw);
	INLINE simd4f		simd4f_splat_w(simd4f xyzw);

	INLINE simd4f		simd4f_shuffle_xxaa(simd4f xyzw, simd4f abcd);
	INLINE simd4f		simd4f_shuffle_xxcc(simd4f xyzw, simd4f abcd);
	INLINE simd4f		simd4f_shuffle_xyab(simd4f xyzw, simd4f abcd);
	INLINE simd4f		simd4f_shuffle_xzac(simd4f xyzw, simd4f abcd);
	INLINE simd4f		simd4f_shuffle_zwab(simd4f xyzw, simd4f abcd);
	INLINE simd4f		simd4f_shuffle_xzac(simd4f xyzw, simd4f abcd);
	INLINE simd4f		simd4f_shuffle_ywbd(simd4f xyzw, simd4f abcd);
	INLINE simd4f		simd4f_shuffle_xxbb(simd4f xyzw, simd4f abcd);
	INLINE simd4f		simd4f_shuffle_zzdd(simd4f xyzw, simd4f abcd);

	INLINE void			PREFETCH(void* ptr, uint32 offset);

	/// @}
}

#ifdef RUSH_USE_INTRINSICS_SSE
#include "SimdIntrinsicsSSE.inl.h"
#endif //RUSH_USE_INTRINSICS_SSE

#ifdef RUSH_USE_INTRINSICS_VMX
#include "SimdIntrinsicsVMX.inl.h"
#endif //RUSH_USE_INTRINSICS_VMX

#ifdef RUSH_USE_INTRINSICS_NEON
#include "SimdIntrinsicsNEON.inl.h"
#endif //RUSH_USE_INTRINSICS_VMX

#ifdef RUSH_USE_SIMD_EMULATION
#include "SimdEmulation.inl.h"
#endif //RUSH_USE_SIMD_EMULATION

