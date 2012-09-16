#pragma once

#include "Rush.h"

/// @addtogroup util
/// @{
#if defined(_M_IX86)
#define LITTLE_ENDIAN
#endif
#if defined(_M_PPC)
#define BIG_ENDIAN
#endif
/// @}

namespace Rush
{
	/// @addtogroup util
	/// @{

	namespace Endianness
	{
		enum type 
		{
			BigEndian,
			LittleEndian
		};
	}

	INLINE void ENDIAN_SWAP_2(void* ptr)
	{
		uint8 tmp;
		uint8* data = (uint8*)ptr;
		tmp = data[0];
		data[0] = data[1];
		data[1] = tmp;
	}

	INLINE void ENDIAN_SWAP_4(void* ptr)
	{
		uint8 tmp;
		uint8* data = (uint8*)ptr;
		tmp = data[0];
		data[0] = data[3];
		data[3] = tmp;
		tmp = data[1];
		data[1] = data[2];
		data[2] = tmp;
	}

	INLINE void ENDIAN_SWAP_8(void* ptr)
	{
		uint8 tmp;
		uint8* data = (uint8*)ptr;
		tmp = data[0];
		data[0] = data[7];
		data[7] = tmp;
		tmp = data[1];
		data[1] = data[6];
		data[6] = tmp;
		tmp = data[2];
		data[2] = data[5];
		data[5] = tmp;
		tmp = data[3];
		data[3] = data[4];
		data[4] = tmp;
	}

	INLINE void ENDIAN_SWAP(uint8&)			{}
	INLINE void ENDIAN_SWAP(uint16& x)		{ENDIAN_SWAP_2(&x);}
	INLINE void ENDIAN_SWAP(uint32& x)		{ENDIAN_SWAP_4(&x);}
	INLINE void ENDIAN_SWAP(uint64& x)		{ENDIAN_SWAP_8(&x);}
	INLINE void ENDIAN_SWAP(int8&)			{}
	INLINE void ENDIAN_SWAP(int16& x)		{ENDIAN_SWAP_2(&x);}
	INLINE void ENDIAN_SWAP(int32& x)		{ENDIAN_SWAP_4(&x);}
	INLINE void ENDIAN_SWAP(int64& x)		{ENDIAN_SWAP_8(&x);}
	INLINE void ENDIAN_SWAP(float& x)		{ENDIAN_SWAP_4(&x);}
	INLINE void ENDIAN_SWAP(double& x)		{ENDIAN_SWAP_8(&x);}
	INLINE void ENDIAN_SWAP(char&)			{}

#ifdef LITTLE_ENDIAN	
	INLINE void TO_LITTLE_ENDIAN(uint8&)		{}
	INLINE void TO_LITTLE_ENDIAN(uint16&)		{}
	INLINE void TO_LITTLE_ENDIAN(uint32&)		{}
	INLINE void TO_LITTLE_ENDIAN(uint64&)		{}
	INLINE void TO_LITTLE_ENDIAN(int8&)			{}
	INLINE void TO_LITTLE_ENDIAN(int16&)		{}
	INLINE void TO_LITTLE_ENDIAN(int32&)		{}
	INLINE void TO_LITTLE_ENDIAN(int64&)		{}
	INLINE void TO_LITTLE_ENDIAN(float&)		{}
	INLINE void TO_LITTLE_ENDIAN(double&)		{}
	INLINE void TO_LITTLE_ENDIAN(char&)			{}
	INLINE void TO_BIG_ENDIAN(uint8&)			{}
	INLINE void TO_BIG_ENDIAN(uint16& x)		{ENDIAN_SWAP_2(&x);} //-V524
	INLINE void TO_BIG_ENDIAN(uint32& x)		{ENDIAN_SWAP_4(&x);} //-V524
	INLINE void TO_BIG_ENDIAN(uint64& x)		{ENDIAN_SWAP_8(&x);} //-V524
	INLINE void TO_BIG_ENDIAN(int8&)			{}
	INLINE void TO_BIG_ENDIAN(int16& x)			{ENDIAN_SWAP_2(&x);} //-V524
	INLINE void TO_BIG_ENDIAN(int32& x)			{ENDIAN_SWAP_4(&x);} //-V524
	INLINE void TO_BIG_ENDIAN(int64& x)			{ENDIAN_SWAP_8(&x);} //-V524
	INLINE void TO_BIG_ENDIAN(float& x)			{ENDIAN_SWAP_4(&x);} //-V524
	INLINE void TO_BIG_ENDIAN(double& x)		{ENDIAN_SWAP_8(&x);} //-V524
	INLINE void TO_BIG_ENDIAN(char&)			{}
#endif //LITTLE_ENDIAN

#ifdef BIG_ENDIAN
	INLINE void TO_LITTLE_ENDIAN(uint8&)		{}
	INLINE void TO_LITTLE_ENDIAN(uint16& x)		{ENDIAN_SWAP_2(&x);}
	INLINE void TO_LITTLE_ENDIAN(uint32& x)		{ENDIAN_SWAP_4(&x);}
	INLINE void TO_LITTLE_ENDIAN(uint64& x)		{ENDIAN_SWAP_8(&x);}
	INLINE void TO_LITTLE_ENDIAN(int8&)			{}
	INLINE void TO_LITTLE_ENDIAN(int16& x)		{ENDIAN_SWAP_2(&x);}
	INLINE void TO_LITTLE_ENDIAN(int32& x)		{ENDIAN_SWAP_4(&x);}
	INLINE void TO_LITTLE_ENDIAN(int64& x)		{ENDIAN_SWAP_8(&x);}
	INLINE void TO_LITTLE_ENDIAN(float& x)		{ENDIAN_SWAP_4(&x);}
	INLINE void TO_LITTLE_ENDIAN(double& x)		{ENDIAN_SWAP_8(&x);}
	INLINE void TO_LITTLE_ENDIAN(char&)			{}
	INLINE void TO_BIG_ENDIAN(uint8&)			{}
	INLINE void TO_BIG_ENDIAN(uint16&)			{}
	INLINE void TO_BIG_ENDIAN(uint32&)			{}
	INLINE void TO_BIG_ENDIAN(uint64&)			{}
	INLINE void TO_BIG_ENDIAN(int8&)			{}
	INLINE void TO_BIG_ENDIAN(int16&)			{}
	INLINE void TO_BIG_ENDIAN(int32&)			{}
	INLINE void TO_BIG_ENDIAN(int64&)			{}
	INLINE void TO_BIG_ENDIAN(float&)			{}
	INLINE void TO_BIG_ENDIAN(double&)			{}
	INLINE void TO_BIG_ENDIAN(char&)			{}
#endif //BIG_ENDIAN

	/// @}
}


