#include "UtilFNV1.h"

namespace Rush
{
	Hasher_FNV1::Hasher_FNV1()
		: m_hash(0x811c9dc5)
	{
	}

	void Hasher_FNV1::hash( const void* _message, uint32 length )
	{
		const uint8* message = (const uint8*)_message;
		const uint32 FNV_PRIME = 16777619;

		for( uint32 i=0; i<length; ++i )
		{
			m_hash *= FNV_PRIME;
			m_hash ^= message[i];
		}
	}
}
