#pragma once

#include "MathPlane.h"
#include "MathMatrix44.h"
#include "Simd.h"

namespace Rush
{
	/// @addtogroup math
	/// @{

	struct FrustumPlane
	{
		enum type
		{
			Left	= 0,
			Right	= 1,
			Top		= 2,
			Bottom	= 3,
			Near	= 4,
			Far		= 5
		};
	};

	class Frustum
	{
	public:
		Frustum();
		Frustum(Matrix44& viewproj);
		~Frustum(){};

		INLINE const Plane& plane(FrustumPlane::type p) const { return m_planes[p]; }
		void set_plane(FrustumPlane::type fp, const Plane& plane);

		INLINE bool intersect_sphere( const Vector3& pos, float radius ) const
		{
			float d;

			d = m_planes[FrustumPlane::Left].distance(pos);
			if( d + radius < 0 ) return false;

			d = m_planes[FrustumPlane::Right].distance(pos);
			if( d + radius < 0 ) return false;

			d = m_planes[FrustumPlane::Top].distance(pos);
			if( d + radius < 0 ) return false;

			d = m_planes[FrustumPlane::Bottom].distance(pos);
			if( d + radius < 0 ) return false;

			d = m_planes[FrustumPlane::Near].distance(pos);
			if( d + radius < 0 ) return false;

			d = m_planes[FrustumPlane::Far].distance(pos);
			if( d + radius < 0 ) return false;

			return true;
		}

		void get_slice_points(float distance, Vector3* out_points) const;

	private:

		Plane	m_planes[6]; //Order: left, right, top, bottom, near, far

	};

	/// @}
}



