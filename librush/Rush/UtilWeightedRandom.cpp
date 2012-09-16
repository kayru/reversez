#include "UtilWeightedRandom.h"

namespace Rush
{
	size_t Rush::WeightedRandom::random_index()
	{
		size_t  start	= m_rand.rand() % m_weights.size();
		float r         = m_rand.get_float(0.0f, m_sum);
		
		float sum		= 0.0f;

		for( size_t i = start; i < m_weights.size(); ++i )
		{
			sum += m_weights[i];
			if( sum >= r ) return i;
		}
		for( size_t i = 0; i < start; ++i )
		{
			sum += m_weights[i];
			if( sum >= r ) return i;
		}

		return 0;
	}


	WeightedRandom::WeightedRandom( uint32 seed, DynamicArray<float>& weights ) 
		: m_rand(seed)
		, m_weights(weights)
		, m_sum(0)
	{				
		for (uint32 i=0; i<m_weights.size(); ++i)
		{
			m_sum += m_weights[i];
		}
	}

	WeightedRandom::~WeightedRandom()
	{
	}
}
