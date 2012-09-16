#pragma once

#include "MathVector3.h"

namespace Rush
{
	/// @addtogroup math
	/// @{

	struct Vector4
	{
		float x, y, z, w;

		//////////////////////////////////////////////////////////////////////////

		INLINE Vector4(){};

		INLINE Vector4(const float _x, const float _y, const float _z, const float _w) : x(_x), y(_y), z(_z), w(_w) {};
		INLINE Vector4(const float s) : x( s ), y( s ), z( s ), w( s ) {};
		INLINE Vector4(const float* arr) : x( arr[0] ), y( arr[1] ), z( arr[2] ), w( arr[3] ) {};
		INLINE Vector4(const Vector4& rhs) : x( rhs.x ), y( rhs.y ), z( rhs.z ), w( rhs.w ) {};
		INLINE Vector4(const Vector3& rhs, float _w=0.0f) : x( rhs.x ), y( rhs.y ), z( rhs.z ), w( _w ) {};

		//////////////////////////////////////////////////////////////////////////

		INLINE Vector4&	operator = ( const float s )				{ x = s; y = s; z = s; w = s; return *this; }

		INLINE Vector4&	operator += ( const Vector4& rhs )			{ x += rhs.x; y += rhs.y; z += rhs.z; w+= rhs.w; return *this; }
		INLINE Vector4&	operator += ( const float s )				{ x += s; y += s; z += s; w += s; return *this; }
		INLINE Vector4&	operator -= ( const Vector4& rhs )			{ x -= rhs.x; y -= rhs.y; z -= rhs.z; w -= rhs.w; return *this; }
		INLINE Vector4&	operator -= ( const float s )				{ x -= s; y -= s; z -= s; w -= s; return *this; }		
		INLINE Vector4&	operator *= ( const Vector4& rhs )			{ x *= rhs.x; y *= rhs.y; z *= rhs.z; w *= rhs.w; return *this; }
		INLINE Vector4&	operator *= ( const float s )				{ x *= s; y *= s; z *= s; w *= s; return *this; }		
		INLINE Vector4&	operator /= ( const Vector4& rhs )			{ x /= rhs.x; y /= rhs.y; z /= rhs.z; w /= rhs.w; return *this; }
		INLINE Vector4&	operator /= ( const float s )				{ x /= s; y /= s; z /= s; w /= s; return *this; }

		INLINE Vector4	operator + ( const Vector4& rhs) const		{ return Vector4(x + rhs.x, y + rhs.y, z + rhs.z, w + rhs.w); }
		INLINE Vector4	operator - ( const Vector4& rhs) const		{ return Vector4(x - rhs.x, y - rhs.y, z - rhs.z, w - rhs.w); }
		INLINE Vector4	operator * ( const Vector4& rhs) const		{ return Vector4(x * rhs.x, y * rhs.y, z * rhs.z, w * rhs.w); }
		INLINE Vector4	operator * ( const float s) const			{ return Vector4(x * s, y * s, z * s, w * s); }
		INLINE Vector4	operator / ( const Vector4& rhs) const		{ return Vector4(x / rhs.x, y / rhs.y, z / rhs.z, w / rhs.w); }
		INLINE Vector4	operator / ( const float s) const			{ return Vector4(x / s, y / s, z / s, w / s); }
		INLINE Vector4	operator + () const							{ return *this; }
		INLINE Vector4	operator - () const							{ return Vector4(-x, -y, -z, -w); }

		INLINE bool		operator == ( const Vector4& rhs ) const	{return ( x == rhs.x && y == rhs.y && z == rhs.z && w == rhs.w ); }
		INLINE bool		operator != ( const Vector4& rhs ) const	{return !(*this==rhs);}
		INLINE bool		operator < ( const Vector4& rhs ) const		{return ( x < rhs.x && y < rhs.y && z < rhs.z && w < rhs.w );}
		INLINE bool		operator <= ( const Vector4& rhs ) const	{return ( x <= rhs.x && y <= rhs.y && z <= rhs.z && w <= rhs.w );}
		INLINE bool		operator > ( const Vector4& rhs ) const		{return ( x > rhs.x && y > rhs.y && z > rhs.z && w > rhs.w );}
		INLINE bool		operator >= ( const Vector4& rhs ) const	{return ( x >= rhs.x && y >= rhs.y && z >= rhs.z && w >= rhs.w );}

		INLINE float	operator [] ( const size_t i ) const		{ return *(&x+i); }
		INLINE float&	operator [] ( const size_t i )				{ return *(&x+i); }

		//////////////////////////////////////////////////////////////////////////

		INLINE	float	length_squared() const { return x*x + y*y + z*z + w*w; }
				float	length() const;

		INLINE	float	dot( const Vector4& rhs ) const { return x*rhs.x + y*rhs.y + z*rhs.z + w*rhs.w; }
		INLINE	void	normalise_this() { (*this)/=length(); }
		INLINE	Vector4	normalised() const { Vector4 res(*this); res.normalise_this(); return res; }
		INLINE	Vector4	reciprocal() const { return Vector4(1.0f/x, 1.0f/y, 1.0f/z, 1.0f/w); }

		INLINE	Vector2	xy()	{ return Vector2(x,y); }
		INLINE	Vector2	xz()	{ return Vector2(x,z); }

		INLINE	const Vector3&	xyz() const { return *((Vector3*)(this)); }
		INLINE	Vector3&		xyz()		{ return *((Vector3*)(this)); }

		INLINE float			swizzle(size_t a) const									{ return array()[a]; }
		INLINE Vector2			swizzle(size_t a, size_t b) const						{ return Vector2(array()[a], array()[b]); }
		INLINE Vector3			swizzle(size_t a, size_t b, size_t c) const				{ return Vector3(array()[a], array()[b], array()[c]); }
		INLINE Vector4			swizzle(size_t a, size_t b, size_t c, size_t d) const	{ return Vector4(array()[a], array()[b], array()[c], array()[d]); }

		INLINE const float*	array() const { return &x; }
		INLINE float*			array() { return &x; }

	};

	INLINE float dot(const Vector4& a, const Vector4& b)
	{
		return a.dot(b);
	}

	INLINE Vector4 operator*(float a, const Vector4& b)
	{
		return b*a;
	}

	/// @}

}

