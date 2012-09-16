#pragma once

namespace Rush
{
	/// @addtogroup util
	/// @{

	template <typename T>
	inline void SafeRelease(T& x)
	{
		if(x)
		{
			x->Release();
			x=NULL; 
		}
	}

	/// @}
}


