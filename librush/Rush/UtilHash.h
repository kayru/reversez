#pragma once

#include "Rush.h"

namespace Rush
{

	/// @addtogroup util
	/// @{

	struct HASH16
	{
		uint16 val;
	};

	struct HASH32
	{
		uint32 val;
	};

	struct MD5Hash
	{
		uint32 data[4];

		inline bool operator == (const MD5Hash& rhs) const
		{
			return 
				data[0] == rhs.data[0] &&
				data[1] == rhs.data[1] &&
				data[2] == rhs.data[2] &&
				data[3] == rhs.data[3];

		}

		inline bool operator != (const MD5Hash& rhs) const
		{
			return !(*this==rhs);
		}

	};

	/// @}

}


