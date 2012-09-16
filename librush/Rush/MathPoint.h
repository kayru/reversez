#pragma once

#include "MathCommon.h"

namespace Rush
{
	/// @addtogroup math
	/// @{

	struct Point2
	{
		int32 x;
		int32 y;

		//////////////////////////////////////////////////////////////////////////

		Point2(){}

		Point2(int32 _x, int32 _y) : x(_x), y(_y) {}

		//////////////////////////////////////////////////////////////////////////

		INLINE bool operator == ( const Point2& rhs ) const	{ return ( x == rhs.x && y == rhs.y ); }
		INLINE bool operator != ( const Point2& rhs ) const	{ return ( x != rhs.x || y != rhs.y ); }

	};

	/// @}
}



