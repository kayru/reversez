#pragma once

#include "Rush.h"

namespace Rush
{
	/// @addtogroup util
	/// @{

	template<typename T, size_t CAPACITY>
	struct StaticArray
	{
		INLINE StaticArray()
			: current_size(0)
		{}

		INLINE T& operator[] (size_t i)
		{
			return data[i];
		}

		INLINE const T& operator[](size_t i) const
		{
			return data[i];
		}

		INLINE const T& back() const
		{
			return data[size()-1];
		}

		INLINE T& back()
		{
			return data[size()-1];
		}

		INLINE bool push_back(const T& val)
		{
			if( current_size < CAPACITY )
			{
				data[current_size] = val;
				++current_size;
				return true;
			}
			else
			{
				return false;
			}
		}

		INLINE bool pop_back()
		{
			if( current_size > 0 )
			{
				--current_size;
				return true;
			}
			else
			{
				return false;
			}
		}

		INLINE void push_back_unsafe(const T& val)
		{
			data[current_size] = val;
			++current_size;
		}

		INLINE void pop_back_unsafe(const T& val)
		{
			--current_size;
		}

		INLINE void clear()
		{
			current_size = 0;
		}

		INLINE size_t capacity() const { return CAPACITY; }
		INLINE size_t size() const { return current_size; }
		INLINE size_t element_size() const { return sizeof(T); }
	
		T data[CAPACITY];
		size_t current_size;
	};

	/// @}
}


