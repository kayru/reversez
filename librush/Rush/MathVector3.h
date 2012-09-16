#pragma once

#include "MathVector2.h"

namespace Rush
{
	/// @addtogroup math
	/// @{

	struct Vector3
	{
		float x, y, z;

		//////////////////////////////////////////////////////////////////////////

		INLINE Vector3(){};

		INLINE Vector3(const float _x, const float _y, const float _z) : x( _x ), y( _y ), z( _z ) {};
		INLINE Vector3(const float s) : x( s ), y( s ), z( s ) {};
		INLINE Vector3(const float* arr) : x( arr[0] ), y( arr[1] ), z( arr[2] ) {};
		INLINE Vector3(const Vector3& rhs) : x( rhs.x ), y( rhs.y ), z( rhs.z ) {};
		INLINE Vector3(const Vector2& rhs, float y) : x( rhs.x ), y( y ), z( rhs.y ) {};

		//////////////////////////////////////////////////////////////////////////
		
		INLINE Vector3&		operator = ( const float s )				{ x = s; y = s; z = s; return *this; }

		INLINE Vector3&		operator += ( const Vector3& rhs )			{ x += rhs.x; y += rhs.y; z += rhs.z; return *this; }
		INLINE Vector3&		operator += ( const float s )				{ x += s; y += s; z += s; return *this; }
		INLINE Vector3&		operator -= ( const Vector3& rhs )			{ x -= rhs.x; y -= rhs.y; z -= rhs.z; return *this; }
		INLINE Vector3&		operator -= ( const float s )				{ x -= s; y -= s; z -= s; return *this; }		
		INLINE Vector3&		operator *= ( const Vector3& rhs )			{ x *= rhs.x; y *= rhs.y; z *= rhs.z; return *this; }
		INLINE Vector3&		operator *= ( const float s )				{ x *= s; y *= s; z *= s; return *this; }		
		INLINE Vector3&		operator /= ( const Vector3& rhs )			{ x /= rhs.x; y /= rhs.y; z /= rhs.z; return *this; }
		INLINE Vector3&		operator /= ( const float s )				{ x /= s; y /= s; z /= s; return *this; }

		INLINE Vector3		operator + ( const Vector3& rhs) const		{ return Vector3(x + rhs.x, y + rhs.y, z + rhs.z); }
		INLINE Vector3		operator - ( const Vector3& rhs) const		{ return Vector3(x - rhs.x, y - rhs.y, z - rhs.z); }
		INLINE Vector3		operator * ( const Vector3& rhs) const		{ return Vector3(x * rhs.x, y * rhs.y, z * rhs.z); }
		INLINE Vector3		operator * ( const float s) const			{ return Vector3(x * s, y * s, z * s); }
		INLINE Vector3		operator / ( const Vector3& rhs) const		{ return Vector3(x / rhs.x, y / rhs.y, z / rhs.z); }
		INLINE Vector3		operator / ( const float s) const			{ return Vector3(x / s, y / s, z / s); }
		INLINE Vector3		operator + () const							{ return *this; }
		INLINE Vector3		operator - () const							{ return Vector3(-x, -y, -z); }

		INLINE bool			operator == ( const Vector3& rhs ) const	{return ( x == rhs.x && y == rhs.y && z == rhs.z ); }
		INLINE bool			operator != ( const Vector3& rhs ) const	{return !(*this==rhs);}
		INLINE bool			operator < ( const Vector3& rhs ) const		{return ( x < rhs.x && y < rhs.y && z < rhs.z );}
		INLINE bool			operator <= ( const Vector3& rhs ) const	{return ( x <= rhs.x && y <= rhs.y && z <= rhs.z );}
		INLINE bool			operator > ( const Vector3& rhs ) const		{return ( x > rhs.x && y > rhs.y && z > rhs.z );}
		INLINE bool			operator >= ( const Vector3& rhs ) const	{return ( x >= rhs.x && y >= rhs.y && z >= rhs.z );}
				
		INLINE float		operator [] ( const size_t i ) const		{ return *(&x+i); }
		INLINE float&		operator [] ( const size_t i )				{ return *(&x+i); }

		//////////////////////////////////////////////////////////////////////////

		INLINE	float		length_squared() const { return x*x + y*y + z*z; }
				float		length() const;

		INLINE	 float		dot( const Vector3& rhs ) const { return x*rhs.x + y*rhs.y + z*rhs.z; }
		INLINE	 Vector3	cross( const Vector3& rhs ) const { return Vector3(y*rhs.z - z*rhs.y, z*rhs.x - x*rhs.z, x*rhs.y - y*rhs.x); }
		INLINE	 Vector3	left() const { return Vector3(-z, y, x); } //left perpendicular
		INLINE	 Vector3	right() const { return Vector3(z, y, -x); } //right perpendicular
		INLINE	 void		normalise_this() { (*this)/=length(); }		
		INLINE	 Vector3	normalised() const { Vector3 res(*this); res.normalise_this(); return res; }
		INLINE	 Vector3	reciprocal() const { return Vector3(1.0f/x, 1.0f/y, 1.0f/z); }
		
		INLINE Vector2		xy() const { return Vector2(x,y); }
		INLINE Vector2		xz() const { return Vector2(x,z); }

		INLINE float		swizzle(size_t a) const								{ return array()[a]; }
		INLINE Vector2		swizzle(size_t a, size_t b) const					{ return Vector2(array()[a], array()[b]); }
		INLINE Vector3		swizzle(size_t a, size_t b, size_t c) const			{ return Vector3(array()[a], array()[b], array()[c]); }

		INLINE const float*	array() const	{ return &x; }
		INLINE float*		array()			{ return &x; }

	};

	INLINE float dot(const Vector3& a, const Vector3& b)
	{
		return a.dot(b);
	}

	INLINE Vector3 cross(const Vector3& a, const Vector3& b)
	{
		return a.cross(b);
	}

	INLINE Vector3 operator*(float a, const Vector3& b)
	{
		return b*a;
	}

	/// @}
}


