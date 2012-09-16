#pragma once

#include "UtilCommon.h"
#include "UtilDynamicArray.h"

namespace Rush
{
	/// @addtogroup util
	/// @{

	// Statically allocated. Can't change size.
	// Fast alloc/dealloc. Non-thread-safe.
	template <typename T, uint32 CAPACITY>
	class StaticPool
	{
		struct Cell
		{
			union
			{
				char	item[sizeof(T)];
				Cell*	next;
			};
		};
	public:

		StaticPool()
		{	
			next = NULL;
			for(uint32 i=0; i<CAPACITY; ++i) dealloc(reinterpret_cast<T*>(&data[i]));
		}

		T* alloc()
		{
			if( next )
			{
				T* res = reinterpret_cast<T*>(next->item);
				next = next->next;
				return res;
			}
			else return NULL;
		}

		void dealloc(T* ptr)
		{
			if( ptr )
			{
				Cell* cell = reinterpret_cast<Cell*>(ptr);
				cell->next = next;
				next = cell;
			}
		}

	private:
		
		Cell	data[CAPACITY];
		Cell*	next;
	};

	/// @}

}


