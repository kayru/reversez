#pragma once

#include "MathCommon.h"

namespace Rush
{
	/// @addtogroup math
	/// @{

	struct Vector2
	{
		float x, y;

		//////////////////////////////////////////////////////////////////////////

		INLINE Vector2(){};

		INLINE Vector2(const float _x, const float _y) : x( _x ), y( _y ) {};		
		INLINE Vector2(const float s) : x( s ), y( s ) {};
		INLINE Vector2(const float* arr)	: x( arr[0] ), y( arr[1] ) {};
		INLINE Vector2(const Vector2& rhs) : x( rhs.x ), y( rhs.y ) {};

		//////////////////////////////////////////////////////////////////////////

		INLINE Vector2&		operator = ( const float s )				{ x = s; y = s; return *this; }

		INLINE Vector2&		operator += ( const Vector2& rhs )			{ x += rhs.x; y += rhs.y; return *this; }
		INLINE Vector2&		operator += ( const float s )				{ x += s; y += s; return *this; }
		INLINE Vector2&		operator -= ( const Vector2& rhs )			{ x -= rhs.x; y -= rhs.y; return *this; }
		INLINE Vector2&		operator -= ( const float s )				{ x -= s; y -= s; return *this; }
		INLINE Vector2&		operator *= ( const Vector2& rhs )			{ x *= rhs.x; y *= rhs.y; return *this; }
		INLINE Vector2&		operator *= ( const float s )				{ x *= s; y *= s; return *this; }
		INLINE Vector2&		operator /= ( const Vector2& rhs )			{ x /= rhs.x; y /= rhs.y; return *this; }
		INLINE Vector2&		operator /= ( const float s )				{ x /= s; y /= s; return *this; }

		INLINE Vector2		operator + ( const Vector2& rhs) const		{ return Vector2(x + rhs.x, y + rhs.y); }
		INLINE Vector2		operator - ( const Vector2& rhs) const		{ return Vector2(x - rhs.x, y - rhs.y); }
		INLINE Vector2		operator * ( const Vector2& rhs) const		{ return Vector2(x * rhs.x, y * rhs.y); }
		INLINE Vector2		operator * ( const float s) const			{ return Vector2(x * s, y * s); }
		INLINE Vector2		operator / ( const Vector2& rhs) const		{ return Vector2(x / rhs.x, y / rhs.y); }
		INLINE Vector2		operator / ( const float s) const			{ return Vector2(x / s, y / s); }
		INLINE Vector2		operator + () const							{ return *this; }
		INLINE Vector2		operator - () const							{ return Vector2(-x, -y); }

		INLINE bool			operator == ( const Vector2& rhs ) const	{ return ( x == rhs.x && y == rhs.y ); }
		INLINE bool			operator != ( const Vector2& rhs ) const	{ return ( x != rhs.x || y != rhs.y ); }
		INLINE bool			operator < ( const Vector2& rhs ) const		{ return ( x < rhs.x && y < rhs.y ); }
		INLINE bool			operator <= ( const Vector2& rhs ) const	{ return ( x <= rhs.x && y <= rhs.y ); }
		INLINE bool			operator > ( const Vector2& rhs ) const		{ return ( x > rhs.x && y > rhs.y ); }
		INLINE bool			operator >= ( const Vector2& rhs ) const	{ return ( x >= rhs.x && y >= rhs.y ); }

		INLINE float		operator [] ( const size_t i ) const		{ return *(&x+i); }
		INLINE float&		operator [] ( const size_t i )				{ return *(&x+i); }
		
		//////////////////////////////////////////////////////////////////////////

		INLINE	float	length_squared() const { return x*x + y*y; }
				float	length() const;

		INLINE	float	dot( const Vector2& rhs ) const { return x*rhs.x + y*rhs.y; }
		INLINE	float	cross( const Vector2& rhs ) const { return x*rhs.y - y*rhs.x; }
		INLINE	Vector2	left(void) const { return Vector2(-y, x); } //left perpendicular
		INLINE	Vector2	right(void) const { return Vector2(y, -x); } //right perpendicular
		INLINE	void	normalise_this() { float len = length(); if(len!=0)(*this)/=len; }
		INLINE	Vector2	normalised() const { Vector2 res(*this); res.normalise_this(); return res; }
		INLINE	Vector2	reflect(const Vector2& normal) { return (*this) - Vector2(2) * normal * Vector2(dot(normal)); }
		INLINE	Vector2	reciprocal() const { return Vector2(1.0f/x, 1.0f/y); }

	};

	INLINE float dot(const Vector2& a, const Vector2& b)
	{
		return a.dot(b);
	}

	INLINE Vector2 cross(const Vector2& a, const Vector2& b)
	{
		return a.cross(b);
	}

	INLINE Vector2 operator*(float a, const Vector2& b)
	{
		return b*a;
	}

	/// @}
}



