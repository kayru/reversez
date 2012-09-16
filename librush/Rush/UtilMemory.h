#pragma once

#include "Rush.h"

namespace Rush
{

	/// @addtogroup util
	/// @{

	struct MemoryBuffer
	{
		inline MemoryBuffer(void* _data, size_t _size)
			: data(_data), size(_size)
		{}

		void*	data;
		size_t	size;
	};

	inline void rush_memcpy(void* dst, const void* src, size_t copy_size);
	inline size_t rush_strlen(const char* data);
	inline int32 rush_strcmp(const char* a, const char*b);
	inline void rush_memset(void* ptr, char val, size_t len);
	#define rush_countof(x) (sizeof(x)/sizeof(x[0]))

	//////////////////////////////////////////////////////////////////////////
	// Inline stuff

	void rush_memcpy(void* dst, const void* src, size_t copy_size)
	{		
		char* dst_data = (char*) dst;
		const char* src_data = (const char*) src;

		for(size_t i=0; i<copy_size; ++i) 
		{
			dst_data[i] = src_data[i];
		}
	}

	size_t rush_strlen(const char* data)
	{
		size_t res = 0;
		while(*data){data++; res++;}
		return res;
	}

	int32 rush_strcmp(const char* a, const char*b)
	{
		while(*a == *b && *a)
		{
			++a;
			++b;
		}
		return *a - *b;
	}

	void rush_memset(void* ptr, char val, size_t len)
	{
		char* data = (char*)ptr;
		for( size_t i=0; i<len; ++i )
		{
			data[i] = val;
		}
	}

	/// @}

}


