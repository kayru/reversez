#pragma once

#include "Rush.h"

namespace Rush
{
	/// @addtogroup util
	/// @{

	struct SHA1_Result
	{
		uint32 data[5];
	};

	//as per http://www.packetizer.com/security/sha1/
	class Hasher_SHA1
	{
	public:

		Hasher_SHA1();
		
		void hash(const uint8* message, uint32 length);

		INLINE const uint32 result(uint32 n) { return m_hash[n]; }
		const SHA1_Result result();

	private:

		INLINE uint32 rotate_left(uint32 x, uint32 bits)
		{
			return (x << bits) | (x >> (32-bits));
		}

		void process_block(const uint8* block); 

		uint32 m_hash[5];

	};

	/// @}

}

