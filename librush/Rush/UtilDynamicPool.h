#pragma once

#include <stdlib.h> // for malloc/free

namespace Rush
{
	/// @addtogroup util
	/// @{

	/// Dynamically allocated. Can change size.
	/// Fast alloc/dealloc unless out of chunks. Non-thread-safe.
	/// When out of chunks, memory for a new chunk is allocated from heap.
	template <typename T, uint32 CHUNK_SIZE=1024>
	class DynamicPool
	{
		struct Cell
		{
			char	item[sizeof(T)];
			Cell*	next;
			bool	active;
		};

		struct Chunk
		{
			Cell	data[CHUNK_SIZE];
			Chunk*	next;
		};

		void extend()
		{
			Chunk* next_chunk = (Chunk*)malloc(sizeof(Chunk));
			next_chunk->next = chunks;
			chunks = next_chunk;
			for(uint32 i=0; i < CHUNK_SIZE; ++i) 
			{
				dealloc(reinterpret_cast<T*>(&next_chunk->data[i]));
			}
		}

		Chunk*	chunks;
		Cell*	next;

	public:

		struct Iterator
		{
			Chunk* chunk;
			uint32 idx;

			Iterator()
			{
				chunk = NULL;
				idx = 0;
			}

			bool valid()
			{
				return chunk!=NULL;
			}

			void advance()
			{
				for(;;)
				{
					++idx;

					if( idx < CHUNK_SIZE )
					{
						if( chunk->data[idx].active )
						{
							break;
						}
					}
					else
					{
						chunk = chunk->next;
						idx = 0;						
						if( chunk == NULL ) break;
						if( chunk->data[idx].active ) break;
					}
				}

			}

			inline T* item()
			{
				return reinterpret_cast<T*>(chunk->data[idx].item);
			}

			inline const T* item() const
			{
				return reinterpret_cast<const T*>(chunk->data[idx].item);
			}

		};

		DynamicPool()
		{
			next = NULL;
			chunks = NULL;
		}

		~DynamicPool()
		{
			while( chunks != NULL )
			{
				Chunk* tmp = chunks;
				chunks = chunks->next;
				free(tmp);
			}
		}

		T* alloc()
		{
			if( next == NULL ) extend();
			next->active = true;
			T* res = reinterpret_cast<T*>(next->item);
			next = next->next;
			return res;
		}

		void dealloc(T* ptr)
		{
			if( ptr )
			{
				Cell* cell = reinterpret_cast<Cell*>(ptr);
				cell->next = next;
				cell->active = false;
				next = cell;
			}
		}

		Iterator begin() const
		{
			Iterator res;
			res.chunk = chunks;
			res.idx = 0;
			
			if( res.chunk )
			{
				if( res.chunk->data[res.idx].active == false )
				{
					res.advance();
				}
			}

			return res;
		}

	};

	/// @}

}


