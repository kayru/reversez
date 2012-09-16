#pragma once

//todo: maybe move this to Util

#include "MathVector4.h"

namespace Rush
{
	/// @addtogroup math
	/// @{

	struct ColourRGBA;
	struct ColourRGBA8;
	
	// high-precision (32 bit floating point) colour
	struct ColourRGBA
	{
		float r, g, b, a;

		INLINE ColourRGBA(){}

		INLINE ColourRGBA(float _r, float _g, float _b, float _a=1.0f)
			: r(_r)
			, g(_g)
			, b(_b)
			, a(_a)
		{
		}

		INLINE ColourRGBA(const Vector3& vec)
			: r(vec.x)
			, g(vec.y)
			, b(vec.z)
			, a(1.0f)
		{
		}

		void rgb_to_hsv();
		void hsv_to_rgb();

		INLINE operator Vector4() const 
		{
			return Vector4(r,g,b,a);
		}

		void apply_gamma(float x);

		operator ColourRGBA8() const;

		//////////////////////////////////////////////////////////////////////////

		static ColourRGBA Black()	{ return ColourRGBA(0.0f, 0.0f, 0.0f); }
		static ColourRGBA White()	{ return ColourRGBA(1.0f, 1.0f, 1.0f); }
		static ColourRGBA Red()		{ return ColourRGBA(1.0f, 0.0f, 0.0f); }
		static ColourRGBA Green()	{ return ColourRGBA(0.0f, 1.0f, 0.0f); }
		static ColourRGBA Blue()	{ return ColourRGBA(0.0f, 0.0f, 1.0f); }
		
		static ColourRGBA Lerp(const ColourRGBA& x, const ColourRGBA& y, float t);

	};


	// low-precision (8 bit unsigned integer) colour
	struct ColourRGBA8
	{
		uint8 r, g, b, a;

		INLINE ColourRGBA8(){}

		explicit ColourRGBA8(uint32 col)
		{
			*this = *reinterpret_cast<ColourRGBA8*>(&col);
		}

		INLINE ColourRGBA8(uint8 _r, uint8 _g, uint8 _b, uint8 _a=255)
			: r(_r)
			, g(_g)
			, b(_b)
			, a(_a)
		{
		}
		
		INLINE operator ColourRGBA() const
		{
			float rf = float(r) / 255.0f;
			float gf = float(g) / 255.0f;
			float bf = float(b) / 255.0f;
			float af = float(a) / 255.0f;

			return ColourRGBA(rf, gf, bf, af);
		}

		operator uint32() const
		{
			return *reinterpret_cast<const uint32*>(this);
		}

		//////////////////////////////////////////////////////////////////////////

		static INLINE ColourRGBA8 Black()	{ return ColourRGBA8(0x00, 0x00, 0x00); }
		static INLINE ColourRGBA8 White()	{ return ColourRGBA8(0xFF, 0xFF, 0xFF); }
		static INLINE ColourRGBA8 Red()		{ return ColourRGBA8(0xFF, 0x00, 0x00); }
		static INLINE ColourRGBA8 Green()	{ return ColourRGBA8(0x00, 0xFF, 0x00); }
		static INLINE ColourRGBA8 Blue()	{ return ColourRGBA8(0x00, 0x00, 0xFF); }

	};

	/// @}
}



