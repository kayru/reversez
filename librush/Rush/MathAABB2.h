#pragma once

#include "MathCommon.h"
#include "MathVector2.h"
#include "MathVector3.h"

namespace Rush
{	
	/// @addtogroup math
	/// @{

	template <typename VECTOR_TYPE>
	class TAABB
	{
	public:

		enum ClampType
		{
			ClampType_None = 0,
			ClampType_MaxX	= 1<<0,
			ClampType_MaxY	= 1<<1,
			ClampType_MaxZ	= 1<<2,
			ClampType_MinX	= 1<<3,
			ClampType_MinY	= 1<<4,
			ClampType_MinZ	= 1<<5
		};

		TAABB()
		{}

		TAABB(const VECTOR_TYPE& min, const VECTOR_TYPE& max) 
			: m_min(min)
			, m_max(max)
		{}

		TAABB(const VECTOR_TYPE& origin, float radius) 
			: m_min(origin-VECTOR_TYPE(radius))
			, m_max(origin+VECTOR_TYPE(radius))
		{}

 		const VECTOR_TYPE& vmin() const { return m_min; }
 		const VECTOR_TYPE& vmax() const { return m_max; }
		VECTOR_TYPE& vmin() { return m_min; }
		VECTOR_TYPE& vmax() { return m_max; }
		const VECTOR_TYPE center() const { return (m_min+m_max) * 0.5f; }
		bool contains(const VECTOR_TYPE& v) const {	return m_min <= v && v <= m_max; }

		VECTOR_TYPE dimensions() const { return m_max-m_min; }	

		void translate(const VECTOR_TYPE& t) { m_min+=t; m_max+=t; }

	protected:

		VECTOR_TYPE m_min;
		VECTOR_TYPE m_max;

	};

	class AABB2 : public TAABB<Vector2>
	{
		typedef TAABB<Vector2> BASE_TYPE;

	public:
		
		AABB2()
		{}

		AABB2(const Vector2& min, const Vector2& max)
			: BASE_TYPE(min, max)
		{}
		AABB2(const Vector2& origin, float radius) 
			: BASE_TYPE(origin, radius)			
		{}
		
		AABB2(float x1, float y1, float x2, float y2)
			: BASE_TYPE(Vector2(x1,y1), Vector2(x2,y2))
		{}

		/// Top Left
		INLINE Vector2 tl() const { return Vector2(m_min.x, m_max.y); }

		/// Top Right
		INLINE Vector2 tr() const { return Vector2(m_max.x, m_max.y); }

		/// Bottom Left
		INLINE Vector2 bl() const { return Vector2(m_min.x, m_min.y); } 

		/// Bottom Right
		INLINE Vector2 br() const { return Vector2(m_max.x, m_min.y); } 

		/// Width
		INLINE float width()  const { return m_max.x - m_min.x; }

		/// Height
		INLINE float height() const { return m_max.y - m_min.y; }

		/// Expands this bounding box to contain the box
		INLINE void expand(const AABB2& rhs )
		{
			m_min.x = rush_min(m_min.x, rhs.m_min.x);
			m_min.y = rush_min(m_min.y, rhs.m_min.y);
			m_max.x = rush_max(m_max.x, rhs.m_max.x);
			m_max.y = rush_max(m_max.y, rhs.m_max.y);
		}

		/// Expands this bounding box to contain the vector
		INLINE void expand(const Vector2& rhs )
		{
			m_min.x = rush_min(m_min.x, rhs.x);
			m_min.y = rush_min(m_min.y, rhs.y);
			m_max.x = rush_max(m_max.x, rhs.x);
			m_max.y = rush_max(m_max.y, rhs.y);
		}

		/// Constrain this bounding box by another, so that all it is fully inside it.
		INLINE void clip(const AABB2& clip_by )
		{
			m_min.x = rush_max(m_min.x, clip_by.m_min.x);
			m_min.y = rush_max(m_min.y, clip_by.m_min.y);
			m_max.x = rush_min(m_max.x, clip_by.m_max.x);
			m_max.y = rush_min(m_max.y, clip_by.m_max.y);
		}

		/// Constrain a given vector by this bounding box.
		/// @return Bit mask, indicating which constraints were applied to the vector.
		INLINE uint8 clamp(Vector2& v) const
		{
			uint8 res = ClampType_None;
			ClampResult::type res_x = rush_clamp(v.x, m_min.x, m_max.x);
			ClampResult::type res_y = rush_clamp(v.y, m_min.y, m_max.y);
			if( res_x == ClampResult::Min )			res |= ClampType_MinX;
			else if( res_x == ClampResult::Max )	res |= ClampType_MaxX;
			if( res_y == ClampResult::Min )			res |= ClampType_MinY;
			else if( res_y == ClampResult::Max )	res |= ClampType_MaxY;
			return res;
		}

	};

	/// @}
}



