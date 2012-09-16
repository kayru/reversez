#pragma once

namespace Rush
{
	/// @addtogroup util
	/// @{

	template <typename T> 
	class StackAllocator
	{
	public:

		StackAllocator()
			: m_data(NULL)
			, m_size(0)
			, m_capacity(0)
		{
		}

		INLINE void setup(T* data, size_t capacity)
		{
			m_data = data;
			m_capacity = capacity;
			m_size = 0;
		}

		INLINE bool can_allocate(size_t count) const
		{
			return (m_size + count) <= m_capacity;
		}

		INLINE T* alloc(size_t count)
		{
			size_t new_size = m_size + count;
			if( new_size <= m_capacity )
			{
				T* res = &m_data[m_size];
				m_size = new_size;
				return res;
			}
			else
			{
				return NULL;
			}
		}

		INLINE T* alloc_unsafe(size_t count)
		{
			T* res = &m_data[m_size];
			m_size += count;
			return res;
		}

		INLINE size_t capacity() const
		{
			return m_capacity;
		}

		INLINE void clear()
		{
			m_size = 0;
		}

		INLINE size_t size() const 
		{
			return m_size;
		}

		INLINE size_t size_in_bytes() const 
		{
			return m_size*sizeof(T);
		}

		INLINE T* data()
		{
			return m_data;
		}

		T*		m_data;
		size_t	m_size;
		size_t	m_capacity;
	};

	/// @}
}


