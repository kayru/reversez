#pragma once

#include "MathAABB2.h"

namespace Rush
{
	/// @addtogroup math
	/// @{

	class AABB3 : public TAABB<Vector3>
	{
		typedef TAABB<Vector3> BASE_TYPE;

	public:

		INLINE AABB3()
		{}

		INLINE AABB3(const Vector3& min, const Vector3& max)
			: BASE_TYPE(min, max)
		{}

		INLINE AABB3(const Vector3& origin, float radius) 
			: BASE_TYPE(origin, radius)
		{}

		/// Expands this bounding box to contain the box
		INLINE void expand(const AABB3& rhs)
		{
			m_min.x = rush_min(m_min.x, rhs.m_min.x);
			m_min.y = rush_min(m_min.y, rhs.m_min.y);
			m_min.z = rush_min(m_min.z, rhs.m_min.z);

			m_max.x = rush_max(m_max.x, rhs.m_max.x);
			m_max.y = rush_max(m_max.y, rhs.m_max.y);
			m_max.z = rush_max(m_max.z, rhs.m_max.z);
		}

		/// Expands this bounding box to contain the vector
		INLINE void expand(const Vector3& rhs)
		{
			m_min.x = rush_min(m_min.x, rhs.x);
			m_min.y = rush_min(m_min.y, rhs.y);
			m_min.z = rush_min(m_min.z, rhs.z);

			m_max.x = rush_max(m_max.x, rhs.x);
			m_max.y = rush_max(m_max.y, rhs.y);
			m_max.z = rush_max(m_max.z, rhs.z);
		}

		/// Constrain a given vector by this bounding box.
		/// @return Bit mask, indicating which constraints were applied to the vector.
		INLINE uint8 clamp(Vector3& v) const
		{
			uint8 res = ClampType_None;
			ClampResult::type res_x = rush_clamp(v.x, m_min.x, m_max.x);
			ClampResult::type res_y = rush_clamp(v.y, m_min.y, m_max.y);
			ClampResult::type res_z = rush_clamp(v.z, m_min.z, m_max.z);
			if( res_x == ClampResult::Min )			res |= ClampType_MinX;
			else if( res_x == ClampResult::Max )	res |= ClampType_MaxX;
			if( res_y == ClampResult::Min )			res |= ClampType_MinY;
			else if( res_y == ClampResult::Max )	res |= ClampType_MaxY;
			if( res_z == ClampResult::Min )			res |= ClampType_MinZ;
			else if( res_z == ClampResult::Max )	res |= ClampType_MaxZ;
			return res;
		}

		INLINE float width()  const { return m_max.x - m_min.x; }
		INLINE float height() const { return m_max.y - m_min.y; }
		INLINE float depth()  const { return m_max.z - m_min.z; }

	};

	/// @}
}



