#pragma once

#include "Rush.h"

namespace Rush
{
	/// @addtogroup math
	/// @{

	template <typename T> INLINE T rush_abs(T i) { return (i < 0) ? -i : i; }

	template <typename T>
	INLINE T rush_lerp(const T& a, const T& b, float alpha)
	{
		return a*(1.0f-alpha)+ b*alpha;
	}

	//////////////////////////////////////////////////////////////////////////

	struct ClampResult
	{
		enum type
		{
			None = 0,
			Min	 = 1<<0,
			Max	 = 1<<1
		};
	};

	template <typename T>
	INLINE ClampResult::type rush_clamp(T& val, T val_min, T val_max)
	{
		if( val < val_min )
		{
			val = val_min;
			return ClampResult::Min;
		}
		if( val > val_max )
		{
			val = val_max;
			return ClampResult::Max;
		}
		return ClampResult::None;
	}

	INLINE float rush_saturate(float val)
	{
		return rush_min(1.0f, rush_max(0.0f, val));
	}

	INLINE float rush_quantize(float val, float step)
	{
		return float(int32(val / step)) * step;
	}

	INLINE uint32 rush_align_ceiling(uint32 x, uint32 boundary)
	{
		return x + (~(x-1) % boundary);
	}

	INLINE uint32 rush_align_floor(uint32 x, uint32 boundary)
	{
		return x - (x % boundary);
	}

    INLINE uint64 rush_align_ceiling(uint64 x, uint64 boundary)
	{
		return x + (~(x-1) % boundary);
	}
    
	INLINE uint64 rush_align_floor(uint64 x, uint64 boundary)
	{
		return x - (x % boundary);
	}
    
	/// @}
}

/// @addtogroup math
/// @{
#define RUSH_ALIGN_CEILING_MACRO(x, boundary) ((x) + (~((x)-1) % boundary))
#define RUSH_ALIGN_FLOOR_MACRO(x, boundary) ((x) - ((x) % boundary))
/// @}



