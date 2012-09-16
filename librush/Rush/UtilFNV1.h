#pragma once

#include "Rush.h"

namespace Rush
{
	/// @addtogroup util
	/// @{

	// as per http://www.isthe.com/chongo/tech/comp/fnv/index.html
	class Hasher_FNV1
	{
	public:

		Hasher_FNV1();

		void hash(const void* message, uint32 length);

		INLINE const uint32 result() { return m_hash; }

	private:

		uint32 m_hash;

	};

	/// @}
}
