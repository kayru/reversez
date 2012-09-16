#pragma once

#include "Rush.h"
#include "UtilAssert.h"
#include "UtilMemory.h"

namespace Rush
{

	/// @addtogroup util
	/// @{

	template <typename T>
	class DynamicArray
	{
	public:

		typedef T ElementType;

		DynamicArray()
			: m_data(NULL)
			, m_used(0)
			, m_capacity(0)
		{
		}

		DynamicArray(const DynamicArray<T>& rhs)
			: m_data(NULL)
			, m_used(0)
			, m_capacity(0)
		{
			reserve(rhs.size());
			for(size_t i=0; i<rhs.size(); ++i)
			{
				m_data[i] = rhs.element(i);
			}
			m_used = rhs.size();
		}

		DynamicArray<T>& operator = (const DynamicArray<T>& rhs)
		{
			reserve(rhs.size());
			for(size_t i=0; i<rhs.size(); ++i)
			{
				m_data[i] = rhs.element(i);
			}
			m_used = rhs.size();
			return *this;
		}

		~DynamicArray()
		{
			delete [] m_data;
		}

		INLINE size_t find(const T& val)
		{
			for( size_t i=0; i<this->size(); ++i )
			{
				if( element(i) == val )
				{
					return i;
				}
			}
			return size_t (-1);
		}

		INLINE bool find_and_remove(const T& val)
		{
			for( size_t i=0; i<this->size(); ++i )
			{
				if( element(i) == val )
				{
					element(i) = this->back();
					this->pop_back();
					return true;
				}
			}
			return false;
		}

		INLINE const T& operator [] (size_t idx) const
		{
			R_ASSERT(idx<size());
			return m_data[idx];
		}

		INLINE T& operator [] (size_t idx)
		{
			R_ASSERT(idx<size());
			return m_data[idx];
		}

		INLINE const T& front() const
		{
			R_ASSERT(size()>0);
			return m_data[0];
		}

		INLINE T& front()
		{
			R_ASSERT(size()>0);
			return m_data[0];
		}

		INLINE void clear()
		{
			m_used = 0;
		}

		INLINE const T& back() const
		{
			R_ASSERT(size()>0);
			return m_data[size()-1];
		}

		INLINE T& back()
		{
			R_ASSERT(size()>0);
			return m_data[size()-1];
		}

		INLINE void reserve(size_t next_capacity)
		{
			if( m_capacity < next_capacity )
			{
				T* new_data = new T[next_capacity];
				for( size_t i=0; i<m_used; ++i )
				{
					new_data[i] = m_data[i];
				}
				delete [] m_data;
				m_data = new_data;
				m_capacity = next_capacity;
			}
		}

		INLINE void resize(size_t next_size)
		{
			if( next_size==0 )
			{
				delete [] m_data;
				m_data = NULL;
				m_used = 0;
				m_capacity = 0;
			}
			else
			{
				reserve(next_size);
				m_used = next_size;
			}
		}

		INLINE bool empty() const 
		{
			return size()==0;
		}

		INLINE size_t size() const 
		{
			return m_used;
		}

		INLINE void pop_back()
		{
			R_ASSERT(size()>0);
			back().~T();
			m_used--;
		}

		INLINE void push_back(const T& val)
		{
			if(m_used == m_capacity)
			{
				reserve(m_used*2+64);
			}
			m_data[m_used] = val;
			m_used++;
		}

		INLINE void remove_at(size_t idx)
		{
			R_ASSERT(idx<size());
			for(size_t i = idx ; i < m_used-1; i++)
			{
				m_data[i] = m_data[i+1];
			}
			m_used--;
		}

		INLINE void unordered_remove_at(size_t idx)
		{
			R_ASSERT(idx<size());
			m_data[idx] = back();
			pop_back();
		}

		INLINE size_t push_back_unique(const T& val)
		{
			size_t idx = find(val);
			if(  idx == size_t (-1) )
			{
				push_back(val);
				return this->size()-1;
			}
			else
			{
				return idx;
			}
		}

		INLINE size_t copy_to(void* output, size_t out_size) const
		{
			if( this->size() )
			{
				size_t this_size_in_bytes = this->size()*sizeof(T);
				size_t copy_size = out_size <= this_size_in_bytes ? out_size : this_size_in_bytes;

				rush_memcpy(output, this->data(), copy_size);
				
				return copy_size;
			}
			else
			{
				return 0;
			}
		}

		INLINE T& element(size_t idx)
		{
			return (*this)[idx];
		}

		INLINE const T& element(size_t idx) const
		{
			return (*this)[idx];
		}

		inline const T* data() const { return m_data; }
		inline T* data() { return m_data; }

		bool operator==(const DynamicArray<T>& rhs) const
		{
			if( this->size() == rhs.size() )
			{
				for( size_t i=0; i<this->size(); ++i )
				{
					if( !((*this)[i] == rhs[i]) ) return false;
				}
				return true;
			}
			return false;
		}

		inline size_t element_size() const { return sizeof(T); }

		// returns pointer to first element
		inline T* begin() { return m_data; }; 

		// returns pointer to element after last
		inline T* end()   { return m_data+m_used; };

	private:

		T*		m_data;
		size_t	m_used;
		size_t	m_capacity;

	};

	/// @}

}


