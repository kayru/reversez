#pragma once

#include "Rush.h"

namespace Rush
{	
	/// @addtogroup util
	/// @{

	/// Thread-safe lock-less single producer single consumer ring buffer queue
	template <uint32 NUM_BLOCKS, typename BLOCK_TYPE>
	class RingBuffer
	{
	public:	
		typedef uint32 INDEX_TYPE;

		RingBuffer()
			: m_tail(0)
			, m_head(0)
		{
		}

		INLINE BLOCK_TYPE* head()
		{	
			INDEX_TYPE tail = m_tail;

			if( m_head+1 == tail || (m_head+1==NUM_BLOCKS && tail==0))
			{
				return NULL;
			}
			else
			{
				return &m_data[m_head];
			}
		}

		INLINE bool produce_head()
		{	
			INDEX_TYPE tail = m_tail;

			if( m_head+1 == tail || (m_head+1==NUM_BLOCKS && tail==0) )
			{
				return false;
			}
			else
			{
				m_head = (m_head+1)%NUM_BLOCKS;
				return true;
			}
		}

		INLINE BLOCK_TYPE* tail()
		{
			INDEX_TYPE head = m_head;

			if( m_tail == head )
			{
				return NULL;
			}
			else
			{
				return &m_data[m_tail];
			}
		}

		INLINE bool consume_tail()
		{
			INDEX_TYPE head = m_head;

			if( m_tail == head )
			{
				return false;
			}
			else
			{
				m_tail = (m_tail+1)%NUM_BLOCKS;
				return true;
			}
		}

		INLINE void clear()
		{
			m_tail = 0;
			m_head = 0;
		}

	private:

		INDEX_TYPE m_tail;
		INDEX_TYPE m_head;
		BLOCK_TYPE m_data[NUM_BLOCKS];

	};

	/// @}

}


