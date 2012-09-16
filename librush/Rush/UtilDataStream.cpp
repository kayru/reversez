#include "UtilDataStream.h"
#include "UtilDynamicArray.h"
#include "MathCommon.h"
#include <string.h>

namespace Rush
{	
	//////////////////////////////////////////////////////////////////////////

	uint32 MemDataStream::tell()
	{
		return m_pos;
	}

	void MemDataStream::seek( uint32 pos )
	{
		m_pos = rush_min(pos, m_size);
	}

	void MemDataStream::skip( int32 distance )
	{
		uint32 pos2 = m_pos+distance;
		pos2 = rush_min(pos2, m_size);
		m_pos = pos2;
	}

	void MemDataStream::rewind()
	{
		m_pos = 0;
	}

	bool MemDataStream::valid()
	{
		return true;
	}

	uint32 MemDataStream::write( const void* buf, uint32 size )
	{
		uint32 pos2 = m_pos+size;
		pos2 = rush_min(pos2, m_size);
		size = pos2-m_pos;
		
		rush_memcpy(&m_data_rw[m_pos], buf, size);
		m_pos = pos2;

		return size;
	}

	uint32 MemDataStream::read( void* buf, uint32 size )
	{

		uint32 pos2 = m_pos+size;
		pos2 = rush_min(pos2, m_size);
		size = pos2 - m_pos;

		rush_memcpy(buf, &m_data_ro[m_pos], size);
		m_pos = pos2;

		return size;
	}

	MemDataStream::MemDataStream(void* data, uint32 size)
		: m_data_rw((uint8*)data)
		, m_data_ro((uint8*)data)
		, m_pos(0)
		, m_size(size)
	{
	}

	MemDataStream::MemDataStream(const void* data, uint32 size)
		: m_data_rw(NULL)
		, m_data_ro((const uint8*)data)
		, m_pos(0)
		, m_size(size)
	{
	}
}
