#pragma once

#include "Rush.h"

namespace Rush
{
	/// @addtogroup util
	/// @{

	INLINE 	uint32 FourCC(uint8 ch0, uint8 ch1, uint8 ch2, uint8 ch3)
	{
		return (uint32(ch0) | (uint32(ch1) << 8) | (uint32(ch2) << 16) | (uint32(ch3) << 24 ));
	}

	/// @}
}



