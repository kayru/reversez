#pragma once

#include "MathCommon.h"
#include "MathVector2.h"
#include "MathVector3.h"

namespace Rush
{
	/// @addtogroup math
	/// @{

	class Line2
	{
	public:

		INLINE Line2()
		{
		}

		INLINE Line2(const Vector2& a, const Vector2& b)
			: start(a)
			, end(b)
		{
		}

		INLINE Line2(float x1, float y1, float x2, float y2) 
			: start(x1,y1)
			, end(x2,y2)
		{
		}
		
		INLINE float length() const
		{
			const Vector2 c = start-end;
			return c.length();
		}

		INLINE float length_squared() const
		{
			const Vector2 c = start-end;
			return c.length_squared();
		}

		int32 intersect(const Line2& rhs, Vector2* pos=0) const;

		float distance(const Vector2& pos) const;

	public:

		Vector2 start;
		Vector2 end;
		
	};


	class Line3
	{
	public:
		
		INLINE Line3()
		{

		}

		INLINE Line3(const Vector3& a, const Vector3& b) 
			: start(a)
			, end(b)
		{

		}

		INLINE Line3(float x1, float y1, float z1, float x2, float y2, float z2) 
			: start(x1, y1, z1)
			, end(x2, y2, z2)
		{

		}

	
	public:

		Vector3 start;
		Vector3 end;

	};

	/// @}
}



