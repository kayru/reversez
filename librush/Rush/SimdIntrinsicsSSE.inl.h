//

namespace Rush
{
	simd4f simd4f_set(float x)
	{
		simd4f res;
		res = _mm_set_ps1(x);
		return res;
	}

	simd4f simd4f_load(const float* arr)
	{
		simd4f res;
		res = _mm_load_ps(arr);
		return res;
	}

	void simd4f_store(float* arr, simd4f v)
	{
		_mm_store_ps(arr,v);
	}

	void simd4f_store_streaming(float* arr, simd4f v)
	{
		_mm_stream_ps(arr,v);
	}

	simd4f simd4f_zero()
	{
		simd4f res;
		res = _mm_setzero_ps();
		return res;
	}

	simd4f simd4f_min(simd4f a, simd4f b)
	{
		simd4f res;
		res = _mm_min_ps(a,b);
		return res;
	}

	simd4f simd4f_max(simd4f a, simd4f b)
	{
		simd4f res;
		res = _mm_max_ps(a,b);
		return res;
	}

	simd4f simd4f_add(simd4f a, simd4f b)
	{
		simd4f res;
		res = _mm_add_ps(a,b);
		return res;
	}

	simd4f simd4f_sub(simd4f a, simd4f b)
	{
		simd4f res;
		res = _mm_sub_ps(a,b);
		return res;
	}

	simd4f simd4f_mul(simd4f a, simd4f b)
	{
		simd4f res;
		res = _mm_mul_ps(a,b);
		return res;
	}

	simd4f simd4f_madd(simd4f a, simd4f b, simd4f c)
	{
		simd4f res;
		res = _mm_mul_ps(a,b);
		res = _mm_add_ps(res,c);
		return res;
	}

	simd4f simd4f_div(simd4f a, simd4f b)
	{		
		simd4f res;
		res = _mm_div_ps(a,b);
		return res;
	}

	simd4f simd4f_rsqrt(simd4f x)
	{
		simd4f res;
		res = _mm_rcp_ps(_mm_sqrt_ps(x));
		return res;
	}

	simd4f simd4f_sqrt(simd4f a)
	{
		simd4f res;
		res = _mm_sqrt_ps(a);
		return res;
	}

	simd4f simd4f_rcp(simd4f x)
	{
		simd4f res;
		res = _mm_rcp_ps(x);
		return res;
	}

	//////////////////////////////////////////////////////////////////////////
	// splats

	simd4f	simd4f_splat_x(simd4f xyzw)
	{
		return _mm_shuffle_ps(xyzw,xyzw,_MM_SHUFFLE(0,0,0,0));
	}
	simd4f	simd4f_splat_y(simd4f xyzw)
	{
		return _mm_shuffle_ps(xyzw,xyzw,_MM_SHUFFLE(1,1,1,1));
	}
	simd4f	simd4f_splat_z(simd4f xyzw)
	{
		return _mm_shuffle_ps(xyzw,xyzw,_MM_SHUFFLE(2,2,2,2));
	}
	simd4f	simd4f_splat_w(simd4f xyzw)
	{
		return _mm_shuffle_ps(xyzw,xyzw,_MM_SHUFFLE(3,3,3,3));
	}

	//////////////////////////////////////////////////////////////////////////
	// shuffles

	simd4f	simd4f_shuffle_xxaa(simd4f xyzw, simd4f abcd)
	{
		return _mm_shuffle_ps(xyzw,abcd,_MM_SHUFFLE(0,0,0,0));
	}

	simd4f	simd4f_shuffle_xxcc(simd4f xyzw, simd4f abcd)
	{
		return _mm_shuffle_ps(xyzw,abcd,_MM_SHUFFLE(2,2,0,0));
	}

	simd4f	simd4f_shuffle_xyab(simd4f xyzw, simd4f abcd)
	{
		return _mm_shuffle_ps(xyzw,abcd,_MM_SHUFFLE(1,0,1,0));
	}

	simd4f	simd4f_shuffle_xzac(simd4f xyzw, simd4f abcd)
	{
		return _mm_shuffle_ps(xyzw,abcd,_MM_SHUFFLE(2,0,2,0));
	}

	simd4f simd4f_shuffle_zwab(simd4f xyzw, simd4f abcd)
	{
		return _mm_shuffle_ps(xyzw,abcd,_MM_SHUFFLE(1,0,3,2));
	}

	simd4f simd4f_shuffle_ywbd(simd4f xyzw, simd4f abcd)
	{
		return _mm_shuffle_ps(xyzw, abcd, _MM_SHUFFLE(3,1,3,1));
	}
	simd4f simd4f_shuffle_xxbb(simd4f xyzw, simd4f abcd)
	{
		return _mm_shuffle_ps(xyzw, abcd, _MM_SHUFFLE(1,1,0,0));
	}
	simd4f simd4f_shuffle_zzdd(simd4f xyzw, simd4f abcd)
	{
		return _mm_shuffle_ps(xyzw, abcd, _MM_SHUFFLE(3,3,2,2));
	}

	void PREFETCH(void* ptr, uint32 offset)
	{
		_mm_prefetch((char*)ptr+offset, _MM_HINT_T0);
	}


}
