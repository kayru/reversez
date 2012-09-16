#include "MathCommon.h"

namespace Rush
{
	INLINE simd4f simd4f_set(float x)
	{
		simd4f res;
		res.f[0] = x;
		res.f[1] = x;
		res.f[2] = x;
		res.f[3] = x;
		return res;
	}

	INLINE simd4f simd4f_load(float* arr)
	{
		simd4f res;
		res.f[0] = arr[0];
		res.f[1] = arr[1];
		res.f[2] = arr[2];
		res.f[3] = arr[3];
		return res;
	}

	INLINE void simd4f_store(float* arr, simd4f v)
	{
		arr[0] = v.f[0];
		arr[1] = v.f[1];
		arr[2] = v.f[2];
		arr[3] = v.f[3];
	}

	INLINE void simd4f_store_streaming(float* arr, simd4f v)
	{
		arr[0] = v.f[0];
		arr[1] = v.f[1];
		arr[2] = v.f[2];
		arr[3] = v.f[3];
	}

	INLINE simd4f simd4f_zero()
	{
		simd4f res;
		res.f[0] = 0;
		res.f[1] = 0;
		res.f[2] = 0;
		res.f[3] = 0;
		return res;
	}

	INLINE simd4f simd4f_min(simd4f a, simd4f b)
	{
		simd4f res;
		res.f[0] = rush_min(a.f[0],b.f[0]);
		res.f[1] = rush_min(a.f[1],b.f[1]);
		res.f[2] = rush_min(a.f[2],b.f[2]);
		res.f[3] = rush_min(a.f[3],b.f[3]);
		return res;
	}

	INLINE simd4f simd4f_max(simd4f a, simd4f b)
	{
		simd4f res;
		res.f[0] = rush_max(a.f[0],b.f[0]);
		res.f[1] = rush_max(a.f[1],b.f[1]);
		res.f[2] = rush_max(a.f[2],b.f[2]);
		res.f[3] = rush_max(a.f[3],b.f[3]);
		return res;
	}

	INLINE simd4f simd4f_add(simd4f a, simd4f b)
	{
		simd4f res;
		res.f[0] = a.f[0]+b.f[0];
		res.f[1] = a.f[1]+b.f[1];
		res.f[2] = a.f[2]+b.f[2];
		res.f[3] = a.f[3]+b.f[3];
		return res;
	}

	INLINE simd4f simd4f_sub(simd4f a, simd4f b)
	{
		simd4f res;
		res.f[0] = a.f[0]-b.f[0];
		res.f[1] = a.f[1]-b.f[1];
		res.f[2] = a.f[2]-b.f[2];
		res.f[3] = a.f[3]-b.f[3];
		return res;
	}

	INLINE simd4f simd4f_mul(simd4f a, simd4f b)
	{
		simd4f res;
		res.f[0] = a.f[0]*b.f[0];
		res.f[1] = a.f[1]*b.f[1];
		res.f[2] = a.f[2]*b.f[2];
		res.f[3] = a.f[3]*b.f[3];
		return res;
	}

	INLINE simd4f simd4f_madd(simd4f a, simd4f b, simd4f c)
	{
		simd4f res;
		res.f[0] = a.f[0]*b.f[0]+c.f[0];
		res.f[1] = a.f[1]*b.f[1]+c.f[1];
		res.f[2] = a.f[2]*b.f[2]+c.f[2];
		res.f[3] = a.f[3]*b.f[3]+c.f[3];
		return res;
	}

	INLINE simd4f simd4f_div(simd4f a, simd4f b)
	{
		simd4f res;
		res.f[0] = a.f[0]/b.f[0];
		res.f[1] = a.f[1]/b.f[1];
		res.f[2] = a.f[2]/b.f[2];
		res.f[3] = a.f[3]/b.f[3];
		return res;
	}

	INLINE simd4f simd4f_rsqrt(simd4f x)
	{
		simd4f res;
		res.f[0] = 1.0f / sqrt(x.f[0]);
		res.f[1] = 1.0f / sqrt(x.f[1]);
		res.f[2] = 1.0f / sqrt(x.f[2]);
		res.f[3] = 1.0f / sqrt(x.f[3]);
		return res;
	}

	simd4f simd4f_sqrt(simd4f x)
	{
		simd4f res;
		res.f[0] = sqrt(x.f[0]);
		res.f[1] = sqrt(x.f[1]);
		res.f[2] = sqrt(x.f[2]);
		res.f[3] = sqrt(x.f[3]);
		return res;
	}

	INLINE simd4f simd4f_rcp(simd4f x)
	{
		simd4f res;
		res.f[0] = 1.0f / x.f[0];
		res.f[1] = 1.0f / x.f[1];
		res.f[2] = 1.0f / x.f[2];
		res.f[3] = 1.0f / x.f[3];
		return res;
	}

	INLINE void PREFETCH(void*, uint32)
	{
	}


	INLINE simd4f simd4f_shuffle_xzac(simd4f xyzw, simd4f abcd)
	{
		simd4f res;
		res.f[0] = xyzw.f[0];
		res.f[1] = xyzw.f[2];
		res.f[2] = abcd.f[0];
		res.f[3] = abcd.f[2];
		return res;
	}

	INLINE simd4f simd4f_shuffle_ywbd(simd4f xyzw, simd4f abcd)
	{
		simd4f res;
		res.f[0] = xyzw.f[1];
		res.f[1] = xyzw.f[3];
		res.f[2] = abcd.f[1];
		res.f[3] = abcd.f[3];
		return res;
	}

	INLINE simd4f simd4f_shuffle_xxbb(simd4f xyzw, simd4f abcd)
	{
		simd4f res;
		res.f[0] = xyzw.f[0];
		res.f[1] = xyzw.f[0];
		res.f[2] = abcd.f[1];
		res.f[3] = abcd.f[1];
		return res;
	}

	INLINE simd4f simd4f_shuffle_zzdd(simd4f xyzw, simd4f abcd)
	{
		simd4f res;
		res.f[0] = xyzw.f[2];
		res.f[1] = xyzw.f[2];
		res.f[2] = abcd.f[3];
		res.f[3] = abcd.f[3];
		return res;
	}
}
