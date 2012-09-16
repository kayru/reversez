#pragma once

#include "Rush.h"

namespace Rush
{
	/// @addtogroup math
	/// @{

	inline float rsqrt_fast(float x)
	{
		typedef union{float f; int32 i;} FLOATINT32_UNION;

		FLOATINT32_UNION a;
		FLOATINT32_UNION b;

		a.f = x*0.5f;
		b.i = 0x5f3759df - (a.i >> 1);
		
		return b.f * (1.5f - a.f * b.f * b.f);
	}

	/// @}
}


