//

namespace Rush
{

	static const uint32 _VECTORMATH_PERM_X=0x00010203L;
	static const uint32 _VECTORMATH_PERM_Y=0x04050607L;
	static const uint32 _VECTORMATH_PERM_Z=0x08090a0bL;
	static const uint32 _VECTORMATH_PERM_W=0x0c0d0e0fL;
	static const uint32 _VECTORMATH_PERM_A=0x10111213L;
	static const uint32 _VECTORMATH_PERM_B=0x14151617L;
	static const uint32 _VECTORMATH_PERM_C=0x18191a1bL;
	static const uint32 _VECTORMATH_PERM_D=0x1c1d1e1fL;

	union uint32_vector4_union
	{
		uint32 u[ 4 ];
		__vector4  v;
		INLINE operator __vector4 () const { return v; }
	};

	simd4f simd4f_set(float x)
	{
		simd4f res;
		res = __vspltw( __lvlx( &x, 0 ), 0 );
		return res;
	}

	simd4f simd4f_load(const float* arr)
	{
		simd4f res;
		res = __lvlx(arr, 0);
		return res;
	}

	void simd4f_store(float* arr, simd4f v)
	{
		__stvx( v, arr, 0 );
	}

	void simd4f_store_streaming(float* arr, simd4f v)
	{
		__stvx( v, arr, 0 );
	}

	simd4f simd4f_zero()
	{
		simd4f res;
		res = __vzero();
		return res;
	}

	simd4f simd4f_min(simd4f a, simd4f b)
	{
		simd4f res;
		res = __vminfp(a,b);
		return res;
	}

	simd4f simd4f_max(simd4f a, simd4f b)
	{
		simd4f res;
		res = __vmaxfp(a,b);
		return res;
	}

	simd4f simd4f_add(simd4f a, simd4f b)
	{
		simd4f res;
		res = __vaddfp(a,b);
		return res;
	}

	simd4f simd4f_sub(simd4f a, simd4f b)
	{
		simd4f res;
		res = __vsubfp(a,b);
		return res;
	}

	simd4f simd4f_mul(simd4f a, simd4f b)
	{
		simd4f res;
		res = __vmulfp(a,b);
		return res;
	}

	simd4f simd4f_madd(simd4f a, simd4f b, simd4f c)
	{
		simd4f res;
		res = __vmaddfp(a,b,c);
		return res;
	}

	simd4f simd4f_div(simd4f a, simd4f b)
	{
		simd4f res;
		res = __vmulfp(a,__vrefp(b));
		return res;
	}

	simd4f simd4f_rsqrt(simd4f x)
	{
		simd4f res;
		res = __vrsqrtefp(x);
		return res;
	}

	simd4f simd4f_sqrt(simd4f x)
	{
		simd4f res;
		res = __vrefp(__vrsqrtefp(x));
		return res;
	}

	simd4f simd4f_rcp(simd4f x)
	{
		simd4f res;
		res = __vrefp(x);
		return res;
	}

	//////////////////////////////////////////////////////////////////////////
	// splats

	simd4f	simd4f_splat_x(simd4f xyzw)
	{
		return __vspltw(xyzw, 0);
	}
	simd4f	simd4f_splat_y(simd4f xyzw)
	{
		return __vspltw(xyzw, 1);
	}
	simd4f	simd4f_splat_z(simd4f xyzw)
	{
		return __vspltw(xyzw, 2);
	}
	simd4f	simd4f_splat_w(simd4f xyzw)
	{
		return __vspltw(xyzw, 3);
	}

	//////////////////////////////////////////////////////////////////////////
	// shuffles

	simd4f	simd4f_shuffle_xxaa(simd4f xyzw, simd4f abcd)
	{
		uint32_vector4_union mask = {_VECTORMATH_PERM_X, _VECTORMATH_PERM_X, _VECTORMATH_PERM_A, _VECTORMATH_PERM_A};
		return __vperm(xyzw,abcd,mask);
	}

	simd4f	simd4f_shuffle_xxcc(simd4f xyzw, simd4f abcd)
	{
		uint32_vector4_union mask = {_VECTORMATH_PERM_X, _VECTORMATH_PERM_X, _VECTORMATH_PERM_C, _VECTORMATH_PERM_C};
		return __vperm(xyzw,abcd,mask);
	}

	simd4f	simd4f_shuffle_xyab(simd4f xyzw, simd4f abcd)
	{
		uint32_vector4_union mask = {_VECTORMATH_PERM_X, _VECTORMATH_PERM_Y, _VECTORMATH_PERM_A, _VECTORMATH_PERM_B};
		return __vperm(xyzw,abcd,mask);
	}

	simd4f	simd4f_shuffle_xzac(simd4f xyzw, simd4f abcd)
	{
		uint32_vector4_union mask = {_VECTORMATH_PERM_X, _VECTORMATH_PERM_Z, _VECTORMATH_PERM_A, _VECTORMATH_PERM_C};
		return __vperm(xyzw,abcd,mask);
	}

	simd4f simd4f_shuffle_zwab(simd4f xyzw, simd4f abcd)
	{
		uint32_vector4_union mask = {_VECTORMATH_PERM_Z, _VECTORMATH_PERM_W, _VECTORMATH_PERM_A, _VECTORMATH_PERM_B};
		return __vperm(xyzw,abcd,mask);
	}

	simd4f simd4f_shuffle_ywbd(simd4f xyzw, simd4f abcd)
	{
		uint32_vector4_union mask_ywbd = {_VECTORMATH_PERM_Y, _VECTORMATH_PERM_W, _VECTORMATH_PERM_B, _VECTORMATH_PERM_D};
		return __vperm(xyzw,abcd,mask_ywbd);
	}

	simd4f simd4f_shuffle_xxbb(simd4f xyzw, simd4f abcd)
	{
		uint32_vector4_union mask_xxbb = {_VECTORMATH_PERM_X, _VECTORMATH_PERM_X, _VECTORMATH_PERM_B, _VECTORMATH_PERM_B};
		return __vperm(xyzw,abcd,mask_xxbb);
	}

	simd4f simd4f_shuffle_zzdd(simd4f xyzw, simd4f abcd)
	{
		uint32_vector4_union mask_zzdd = {_VECTORMATH_PERM_Z, _VECTORMATH_PERM_Z, _VECTORMATH_PERM_D, _VECTORMATH_PERM_D};
		return __vperm(xyzw,abcd,mask_zzdd);
	}

	void PREFETCH(void* ptr, uint32 offset)
	{
		__dcbt(offset,ptr);
	}

}
