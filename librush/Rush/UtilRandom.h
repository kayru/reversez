#pragma once

#include "Rush.h"

namespace Rush
{
	/// @addtogroup util
	/// @{

	class Rand
	{
	public:

		Rand(uint32 seed=0) : m_seed(seed) {}

		INLINE uint32	rand();

		INLINE float	get_float(float min, float max);
		INLINE int32	get_int32(int32 min, int32 max);
		INLINE uint32	get_uint32(uint32 min, uint32 max);
		INLINE uint8	get_uint8(uint8 min, uint8 max);

	private:

		uint32 m_seed;

	};

	//////////////////////////////////////////////////////////////////////////
	// inline stuff

	float Rand::get_float( float min, float max )
	{
		m_seed=214013*m_seed+2531011;
		return min+(m_seed>>16)*(1.0f/65535.0f)*(max-min);
	}

	int32 Rand::get_int32( int32 min, int32 max )
	{
		return min+rand()%(max-min+1);
	}

	uint32 Rand::get_uint32( uint32 min, uint32 max )
	{
		return min+rand()%(max-min+1);
	}

	uint8 Rand::get_uint8( uint8 min, uint8 max )
	{
		return min+uint8(rand()%(max-min+1));
	}
	
	uint32 Rand::rand()
	{
		m_seed=214013*m_seed+2531011;
		return (m_seed ^ m_seed>>16);
	}


	/// @}
}



