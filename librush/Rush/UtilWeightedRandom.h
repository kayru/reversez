#pragma once

#include "UtilRandom.h"
#include "UtilDynamicArray.h"

namespace Rush
{
	/// @addtogroup util
	/// @{

	class WeightedRandom
	{
	public:	

		WeightedRandom(uint32 seed, DynamicArray<float>& weights);
		~WeightedRandom();

		size_t random_index();
		
	private:

		Rand					m_rand;
		DynamicArray<float>     m_weights;
		float					m_sum;
		
	};

	/// @}

}


