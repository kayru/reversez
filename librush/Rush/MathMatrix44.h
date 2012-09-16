#pragma once

#include "MathVector3.h"
#include "MathVector4.h"

namespace Rush
{
	/// @addtogroup math
	/// @{

	// row-major matrix
	struct Matrix44
	{
		Vector4 rows[4];

		INLINE Matrix44(){};

		INLINE Matrix44(
			float m00, float m10, float m20, float m30,
			float m01, float m11, float m21, float m31,
			float m02, float m12, float m22, float m32,
			float m03, float m13, float m23, float m33 );

		INLINE Matrix44(
			float m00, float m10, float m20,
			float m01, float m11, float m21,
			float m02, float m12, float m22);

		INLINE Matrix44( float* arr );

		//////////////////////////////////////////////////////////////////////////

		Matrix44				operator*(const Matrix44& rhs) const;
		Matrix44&				operator*=(const Matrix44& rhs);

		INLINE float&			operator()(size_t r, size_t c);
		INLINE const float&     operator()(size_t r, size_t c) const;

		//////////////////////////////////////////////////////////////////////////

		INLINE Vector4			column(size_t c) const;

		INLINE Vector4&			row(size_t r);
		INLINE const Vector4&	row(size_t r) const;

		INLINE void				set_row(size_t r, const Vector4& v);

		INLINE float&			m(size_t r, size_t c);
		INLINE const float&     m(size_t r, size_t c) const;

		INLINE Matrix44			transposed() const;
		INLINE void				transpose();

		float					det() const;

		Matrix44				inverse() const;

		Matrix44				multiply33(const Matrix44& mat);

		INLINE Vector4			transform_vector(const Vector4& v) const;
		INLINE Vector3			transform_vector(const Vector3& v3) const;

		//////////////////////////////////////////////////////////////////////////

		INLINE static Matrix44	zero();
		INLINE static Matrix44	identity();

	};

	/// @}
}

#include "MathMatrix44.inl.h"
