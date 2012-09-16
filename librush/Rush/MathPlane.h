#pragma once

#include "MathApprox.h"
#include "MathVector3.h"
#include "MathVector4.h"

namespace Rush
{
	/// @addtogroup math
	/// @{


	struct Plane : public Vector4
	{
		INLINE	Plane();
		INLINE	Plane(const Vector3&v, float _w);
		INLINE	Plane(float _x, float _y, float _z, float _w);
		INLINE	Plane( const Vector3& pos, const Vector3& normal);
		INLINE	Vector3 normal() const;
		INLINE	float distance(const Vector3& pos) const;		
				void normalize();
				void normalize_fast();
		INLINE	float dot(const Vector3& rhs) const;
		INLINE	float dot(const Plane& rhs) const;
		INLINE	Vector3 cross(const Plane& rhs) const;
		
		Vector3	intersect_infinite_line(const Vector3& p0, const Vector3& p1);

		static bool intersect3(const Plane& a, const Plane& b, const Plane& c, Vector3& out_point);

	};

	//////////////////////////////////////////////////////////////////////////
	// inline stuff

	Plane::Plane() : Vector4()
	{
	}

	Plane::Plane( const Vector3&v, float _w ) 
		: Vector4(v.x,v.y,v.z,_w)
	{

	}

	Plane::Plane( float _x, float _y, float _z, float _w ) 
		: Vector4(_x,_y,_z,_w)
	{

	}

	Plane::Plane( const Vector3& pos, const Vector3& normal)
	{
		x = normal.x;
		y = normal.y;
		z = normal.z;
		w = -(normal.dot(pos));
	}

	Vector3 Plane::normal() const
	{
		return Vector3(x,y,z);
	}

	float Plane::distance( const Vector3& pos ) const
	{
		return x * pos.x + y * pos.y + z * pos.z + w;
	}

	float Plane::dot( const Vector3& rhs ) const
	{
		return normal().dot(rhs);
	}

	float Plane::dot( const Plane& rhs ) const
	{
		return normal().dot(rhs.normal());
	}

	Vector3 Plane::cross( const Plane& rhs ) const
	{
		return normal().cross(rhs.normal());
	}

	/// @}
}

// EOF
