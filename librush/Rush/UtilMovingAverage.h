#pragma once

#include "Rush.h"

namespace Rush
{	

	/// @addtogroup util
	/// @{

	template <uint32 SIZE>
	struct MovingAverage
	{
		MovingAverage()
		{
			reset();
		}

		inline void reset()
		{
			idx = 0;
			sum = 0;
			for( uint32 i=0; i<SIZE; ++i )
			{
				buf[i] = 0.0f;
			}
		}
	
		inline void add(float v)
		{
			sum += v;
			sum -= buf[idx];
			buf[idx] = v;
			idx = (idx+1)%SIZE;
		}
	
		inline float get() const
		{
			return sum / SIZE;
		}

		uint32 idx;
		float sum;
		float buf[SIZE];
	};	

	/// @}

}


