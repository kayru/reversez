//

namespace Rush
{
    union simd4f_union
    {
        simd4f v;
        float  f[4];
    };
    
	simd4f simd4f_set(float x)
	{
		simd4f res;
		res = vdupq_n_f32(x);
		return res;
	}

	simd4f simd4f_load(const float* arr)
	{
		simd4f res;
		res = vld1q_f32(arr);
		return res;
	}

	void simd4f_store(float* arr, simd4f v)
	{
		vst1q_f32(arr,v);
	}

	void simd4f_store_streaming(float* arr, simd4f v)
	{
		simd4f_store(arr,v);
	}

	simd4f simd4f_zero()
	{
		simd4f res;
		res = vdupq_n_f32(0.0f);
		return res;
	}

	simd4f simd4f_min(simd4f a, simd4f b)
	{
		simd4f res;
		res = vminq_f32(a,b);
		return res;
	}

	simd4f simd4f_max(simd4f a, simd4f b)
	{
		simd4f res;
		res = vmaxq_f32(a,b);
		return res;
	}

	simd4f simd4f_add(simd4f a, simd4f b)
	{
		simd4f res;
		res = vaddq_f32(a,b);
		return res;
	}

	simd4f simd4f_sub(simd4f a, simd4f b)
	{
		simd4f res;
		res = vsubq_f32(a,b);
		return res;
	}

	simd4f simd4f_mul(simd4f a, simd4f b)
	{
		simd4f res;
		res = vmulq_f32(a,b);
		return res;
	}

	simd4f simd4f_madd(simd4f a, simd4f b, simd4f c)
	{
		simd4f res;
		res = vmlaq_f32(c,a,b);
		return res;
	}

	simd4f simd4f_div(simd4f a, simd4f b)
	{		
		simd4f res;
        res = vmulq_f32(a,vrecpeq_f32(b));
		return res;
	}

	simd4f simd4f_rsqrt(simd4f x)
	{
		simd4f res;
		res = vrsqrteq_f32(x);
		return res;
	}

	simd4f simd4f_sqrt(simd4f a)
	{
		simd4f res;
		res = vrecpeq_f32(vrsqrteq_f32(a));
		return res;
	}

	simd4f simd4f_rcp(simd4f x)
	{
		simd4f res;
		res = vrecpeq_f32(x);
		return res;
	}

	//////////////////////////////////////////////////////////////////////////
	// splats

	simd4f	simd4f_splat_x(simd4f xyzw)
	{        
		return vdupq_lane_f32(vget_low_f32(xyzw),0);
	}
	simd4f	simd4f_splat_y(simd4f xyzw)
	{
		return vdupq_lane_f32(vget_low_f32(xyzw),1);
	}
	simd4f	simd4f_splat_z(simd4f xyzw)
	{
		return vdupq_lane_f32(vget_high_f32(xyzw),0);
	}
	simd4f	simd4f_splat_w(simd4f xyzw)
	{
		return vdupq_lane_f32(vget_high_f32(xyzw),0);
	}

	//////////////////////////////////////////////////////////////////////////
	// shuffles

	simd4f	simd4f_shuffle_xxaa(simd4f xyzw, simd4f abcd)
	{
        simd4f_union u0 = {xyzw};
        simd4f_union u1 = {abcd};        
        float r[4] = {u0.f[0], u0.f[0], u1.f[0], u1.f[0]};  
        return simd4f_load(r);
	}

	simd4f	simd4f_shuffle_xxcc(simd4f xyzw, simd4f abcd)
	{
        simd4f_union u0 = {xyzw};
        simd4f_union u1 = {abcd};        
        float r[4] = {u0.f[0], u0.f[0], u1.f[2], u1.f[2]};    
        return simd4f_load(r);
	}

	simd4f	simd4f_shuffle_xyab(simd4f xyzw, simd4f abcd)
	{
        simd4f_union u0 = {xyzw};
        simd4f_union u1 = {abcd};        
        float r[4] = {u0.f[0], u0.f[1], u1.f[0], u1.f[1]};    
        return simd4f_load(r);
	}

	simd4f	simd4f_shuffle_xzac(simd4f xyzw, simd4f abcd)
	{
        simd4f_union u0 = {xyzw};
        simd4f_union u1 = {abcd};        
        float r[4] = {u0.f[0], u0.f[2], u1.f[0], u1.f[2]};    
        return simd4f_load(r);
	}

	simd4f simd4f_shuffle_zwab(simd4f xyzw, simd4f abcd)
	{
        simd4f_union u0 = {xyzw};
        simd4f_union u1 = {abcd};        
        float r[4] = {u0.f[2], u0.f[3], u1.f[0], u1.f[1]};    
        return simd4f_load(r);
	}

	simd4f simd4f_shuffle_ywbd(simd4f xyzw, simd4f abcd)
	{
        simd4f_union u0 = {xyzw};
        simd4f_union u1 = {abcd};        
        float r[4] = {u0.f[1], u0.f[3], u1.f[1], u1.f[3]};    
        return simd4f_load(r);
	}
	simd4f simd4f_shuffle_xxbb(simd4f xyzw, simd4f abcd)
	{
        simd4f_union u0 = {xyzw};
        simd4f_union u1 = {abcd};        
        float r[4] = {u0.f[0], u0.f[0], u1.f[1], u1.f[1]};    
        return simd4f_load(r);
	}
	simd4f simd4f_shuffle_zzdd(simd4f xyzw, simd4f abcd)
	{
        simd4f_union u0 = {xyzw};
        simd4f_union u1 = {abcd};        
        float r[4] = {u0.f[2], u0.f[2], u1.f[3], u1.f[3]};    
        return simd4f_load(r);
	}

	void PREFETCH(void* /*ptr*/, uint32 /*offset*/)
	{
		
	}


}
