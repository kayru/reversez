#pragma once

#include "Rush.h"
#include "UtilDynamicArray.h"
#include "UtilStaticArray.h"

#include <stdlib.h>

namespace Rush
{
	/// @addtogroup util
	/// @{

	// Dynamically allocated. Can change size.
	// Fast alloc/dealloc unless out of free cells.
	// In which case it allocates memory for cells like standard DynamicArray.
	// Non-thread-safe.
	// Allocs / deallocs don't invalidate order of surrounding elements. 	
	template <typename T>
	class IndexedPool
	{
	public:
		INLINE size_t push(const T& val)
		{
			if( empty.empty() )
			{
				used.push_back(true);
				data.push_back(val);
				return data.size()-1;
			}
			else
			{
				size_t idx = empty.back();
				empty.pop_back();
				used[idx] = true;
				data[idx] = val;
				return idx;
			}
		}

		INLINE void remove(size_t idx)
		{			
			if( used[idx] == true )
			{
				used[idx] = false;
				empty.push_back(idx);
			}			
		}

		INLINE T* element_unsafe(size_t idx)
		{
			return &data[idx];
		}

		INLINE const T* element_unsafe(size_t idx) const
		{
			return &data[idx];
		}

		INLINE T* element(size_t idx)
		{
			if( idx < data.size() )
			{
				if( used[idx] ) return &data[idx];
				else return NULL;
			}
			else return NULL;
		}

		INLINE const T* element(size_t idx) const
		{
			if( idx < data.size() )
			{
				if( used[idx] ) return &data[idx];
				else return NULL;
			}
			else return NULL;
		}

		INLINE size_t size() const { return data.size(); }

	private:

		DynamicArray<T>			data;
		DynamicArray<bool>		used;
		DynamicArray<size_t>	empty;

	};

	// Statically allocated. Can NOT change size.
	// Always fast alloc/dealloc.
	// Non-thread-safe.
	// Allocs / deallocs don't invalidate order of surrounding elements. 	
	template <typename T, size_t CAPACITY>
	class StaticIndexedPool
	{
	public:

		StaticIndexedPool()
		{
			for(size_t i=CAPACITY; i!=0; --i)
			{
				empty.push_back(i-1);
			}
		}

		INLINE size_t push(const T& val)
		{
			R_ASSERT(empty.size()!=0);
			size_t idx = empty.back();
			empty.pop_back();
			data[idx] = val;
			return idx;
		}

		INLINE void remove(size_t idx)
		{	
			empty.push_back(idx);
		}

		INLINE const T& operator [] (size_t idx) const
		{
			return data[idx];
		}

		INLINE T& operator [] (size_t idx)
		{
			return data[idx];
		}

		INLINE size_t capacity() const { return CAPACITY; }

	private:

		T data[CAPACITY];
		StaticArray<size_t, CAPACITY>	empty;

	};

	/// @}

}


