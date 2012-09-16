#include "MathFrustum.h"

namespace Rush
{
	Frustum::Frustum()
	{
	}

	Frustum::Frustum( Matrix44& viewproj )
	{	
		m_planes[FrustumPlane::Left].x = viewproj(0,3) + viewproj(0,0);
		m_planes[FrustumPlane::Left].y = viewproj(1,3) + viewproj(1,0);
		m_planes[FrustumPlane::Left].z = viewproj(2,3) + viewproj(2,0);
		m_planes[FrustumPlane::Left].w = viewproj(3,3) + viewproj(3,0);

		m_planes[FrustumPlane::Right].x = viewproj(0,3) - viewproj(0,0);
		m_planes[FrustumPlane::Right].y = viewproj(1,3) - viewproj(1,0);
		m_planes[FrustumPlane::Right].z = viewproj(2,3) - viewproj(2,0);
		m_planes[FrustumPlane::Right].w = viewproj(3,3) - viewproj(3,0);

		//////////////////////////////////////////////////////////////////////////

		m_planes[FrustumPlane::Top].x = viewproj(0,3) - viewproj(0,1);
		m_planes[FrustumPlane::Top].y = viewproj(1,3) - viewproj(1,1);
		m_planes[FrustumPlane::Top].z = viewproj(2,3) - viewproj(2,1);
		m_planes[FrustumPlane::Top].w = viewproj(3,3) - viewproj(3,1);

		m_planes[FrustumPlane::Bottom].x = viewproj(0,3) + viewproj(0,1);
		m_planes[FrustumPlane::Bottom].y = viewproj(1,3) + viewproj(1,1);
		m_planes[FrustumPlane::Bottom].z = viewproj(2,3) + viewproj(2,1);
		m_planes[FrustumPlane::Bottom].w = viewproj(3,3) + viewproj(3,1);

		//////////////////////////////////////////////////////////////////////////

		m_planes[FrustumPlane::Near].x = viewproj(0,2);
		m_planes[FrustumPlane::Near].y = viewproj(1,2);
		m_planes[FrustumPlane::Near].z = viewproj(2,2);
		m_planes[FrustumPlane::Near].w = viewproj(3,2);

		m_planes[FrustumPlane::Far].x = viewproj(0,3) - viewproj(0,2);
		m_planes[FrustumPlane::Far].y = viewproj(1,3) - viewproj(1,2);
		m_planes[FrustumPlane::Far].z = viewproj(2,3) - viewproj(2,2);
		m_planes[FrustumPlane::Far].w = viewproj(3,3) - viewproj(3,2);

		//////////////////////////////////////////////////////////////////////////

		for( uint32 i=0; i<6; ++i )
		{
			m_planes[i].normalize_fast();
		}	
	}

	void Frustum::set_plane( FrustumPlane::type fp, const Plane& plane )
	{
		m_planes[fp] = plane;
	}

	void Frustum::get_slice_points( float distance, Vector3* out_points ) const
	{
		const Plane& n = m_planes[FrustumPlane::Near];
		Plane slice(n.x, n.y, n.z, n.w - distance);

		Plane::intersect3(slice, plane(FrustumPlane::Left),		plane(FrustumPlane::Top),		out_points[0]);
		Plane::intersect3(slice, plane(FrustumPlane::Right),	plane(FrustumPlane::Top),		out_points[1]);
		Plane::intersect3(slice, plane(FrustumPlane::Left),		plane(FrustumPlane::Bottom),	out_points[2]);
		Plane::intersect3(slice, plane(FrustumPlane::Right),	plane(FrustumPlane::Bottom),	out_points[3]);
	}
}
