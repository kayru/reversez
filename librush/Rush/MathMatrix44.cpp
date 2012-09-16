#include "MathMatrix44.h"

namespace Rush
{
	INLINE void mul33(Matrix44& res, const Matrix44& a, const Matrix44& b)
	{
		res.m(0,0) = a.m(0,0) * b.m(0,0) +a.m(0,1) * b.m(1,0) +a.m(0,2) * b.m(2,0);
		res.m(0,1) = a.m(0,0) * b.m(0,1) +a.m(0,1) * b.m(1,1) +a.m(0,2) * b.m(2,1);
		res.m(0,2) = a.m(0,0) * b.m(0,2) +a.m(0,1) * b.m(1,2) +a.m(0,2) * b.m(2,2);

		res.m(1,0) = a.m(1,0) * b.m(0,0) +a.m(1,1) * b.m(1,0) +a.m(1,2) * b.m(2,0);
		res.m(1,1) = a.m(1,0) * b.m(0,1) +a.m(1,1) * b.m(1,1) +a.m(1,2) * b.m(2,1);
		res.m(1,2) = a.m(1,0) * b.m(0,2) +a.m(1,1) * b.m(1,2) +a.m(1,2) * b.m(2,2);

		res.m(2,0) = a.m(2,0) * b.m(0,0) +a.m(2,1) * b.m(1,0) +a.m(2,2) * b.m(2,0);
		res.m(2,1) = a.m(2,0) * b.m(0,1) +a.m(2,1) * b.m(1,1) +a.m(2,2) * b.m(2,1);
		res.m(2,2) = a.m(2,0) * b.m(0,2) +a.m(2,1) * b.m(1,2) +a.m(2,2) * b.m(2,2);
	}

	INLINE void mul(Matrix44& res, const Matrix44& a, const Matrix44& b)
	{
		res.m(0,0) = a.m(0,0) * b.m(0,0) +a.m(0,1) * b.m(1,0) +a.m(0,2) * b.m(2,0) +a.m(0,3) * b.m(3,0);
		res.m(0,1) = a.m(0,0) * b.m(0,1) +a.m(0,1) * b.m(1,1) +a.m(0,2) * b.m(2,1) +a.m(0,3) * b.m(3,1);
		res.m(0,2) = a.m(0,0) * b.m(0,2) +a.m(0,1) * b.m(1,2) +a.m(0,2) * b.m(2,2) +a.m(0,3) * b.m(3,2);
		res.m(0,3) = a.m(0,0) * b.m(0,3) +a.m(0,1) * b.m(1,3) +a.m(0,2) * b.m(2,3) +a.m(0,3) * b.m(3,3);

		res.m(1,0) = a.m(1,0) * b.m(0,0) +a.m(1,1) * b.m(1,0) +a.m(1,2) * b.m(2,0) +a.m(1,3) * b.m(3,0);
		res.m(1,1) = a.m(1,0) * b.m(0,1) +a.m(1,1) * b.m(1,1) +a.m(1,2) * b.m(2,1) +a.m(1,3) * b.m(3,1);
		res.m(1,2) = a.m(1,0) * b.m(0,2) +a.m(1,1) * b.m(1,2) +a.m(1,2) * b.m(2,2) +a.m(1,3) * b.m(3,2);
		res.m(1,3) = a.m(1,0) * b.m(0,3) +a.m(1,1) * b.m(1,3) +a.m(1,2) * b.m(2,3) +a.m(1,3) * b.m(3,3);

		res.m(2,0) = a.m(2,0) * b.m(0,0) +a.m(2,1) * b.m(1,0) +a.m(2,2) * b.m(2,0) +a.m(2,3) * b.m(3,0);
		res.m(2,1) = a.m(2,0) * b.m(0,1) +a.m(2,1) * b.m(1,1) +a.m(2,2) * b.m(2,1) +a.m(2,3) * b.m(3,1);
		res.m(2,2) = a.m(2,0) * b.m(0,2) +a.m(2,1) * b.m(1,2) +a.m(2,2) * b.m(2,2) +a.m(2,3) * b.m(3,2);
		res.m(2,3) = a.m(2,0) * b.m(0,3) +a.m(2,1) * b.m(1,3) +a.m(2,2) * b.m(2,3) +a.m(2,3) * b.m(3,3);

		res.m(3,0) = a.m(3,0) * b.m(0,0) +a.m(3,1) * b.m(1,0) +a.m(3,2) * b.m(2,0) +a.m(3,3) * b.m(3,0);
		res.m(3,1) = a.m(3,0) * b.m(0,1) +a.m(3,1) * b.m(1,1) +a.m(3,2) * b.m(2,1) +a.m(3,3) * b.m(3,1);
		res.m(3,2) = a.m(3,0) * b.m(0,2) +a.m(3,1) * b.m(1,2) +a.m(3,2) * b.m(2,2) +a.m(3,3) * b.m(3,2);
		res.m(3,3) = a.m(3,0) * b.m(0,3) +a.m(3,1) * b.m(1,3) +a.m(3,2) * b.m(2,3) +a.m(3,3) * b.m(3,3);
	}

	Matrix44 Matrix44::inverse() const
	{
		float m00 = m(0,0), m01 = m(0,1), m02 = m(0,2), m03 = m(0,3);
		float m10 = m(1,0), m11 = m(1,1), m12 = m(1,2), m13 = m(1,3);
		float m20 = m(2,0), m21 = m(2,1), m22 = m(2,2), m23 = m(2,3);
		float m30 = m(3,0), m31 = m(3,1), m32 = m(3,2), m33 = m(3,3);

		float v0 = m20 * m31 - m21 * m30;
		float v1 = m20 * m32 - m22 * m30;
		float v2 = m20 * m33 - m23 * m30;
		float v3 = m21 * m32 - m22 * m31;
		float v4 = m21 * m33 - m23 * m31;
		float v5 = m22 * m33 - m23 * m32;

		float t00 = + (v5 * m11 - v4 * m12 + v3 * m13);
		float t10 = - (v5 * m10 - v2 * m12 + v1 * m13);
		float t20 = + (v4 * m10 - v2 * m11 + v0 * m13);
		float t30 = - (v3 * m10 - v1 * m11 + v0 * m12);

		float det = (t00 * m00 + t10 * m01 + t20 * m02 + t30 * m03);
		float rdet = 1.0f / det;

		float d00 = t00 * rdet;
		float d10 = t10 * rdet;
		float d20 = t20 * rdet;
		float d30 = t30 * rdet;

		float d01 = - (v5 * m01 - v4 * m02 + v3 * m03) * rdet;
		float d11 = + (v5 * m00 - v2 * m02 + v1 * m03) * rdet;
		float d21 = - (v4 * m00 - v2 * m01 + v0 * m03) * rdet;
		float d31 = + (v3 * m00 - v1 * m01 + v0 * m02) * rdet;

		v0 = m10 * m31 - m11 * m30;
		v1 = m10 * m32 - m12 * m30;
		v2 = m10 * m33 - m13 * m30;
		v3 = m11 * m32 - m12 * m31;
		v4 = m11 * m33 - m13 * m31;
		v5 = m12 * m33 - m13 * m32;

		float d02 = + (v5 * m01 - v4 * m02 + v3 * m03) * rdet;
		float d12 = - (v5 * m00 - v2 * m02 + v1 * m03) * rdet;
		float d22 = + (v4 * m00 - v2 * m01 + v0 * m03) * rdet;
		float d32 = - (v3 * m00 - v1 * m01 + v0 * m02) * rdet;

		v0 = m21 * m10 - m20 * m11;
		v1 = m22 * m10 - m20 * m12;
		v2 = m23 * m10 - m20 * m13;
		v3 = m22 * m11 - m21 * m12;
		v4 = m23 * m11 - m21 * m13;
		v5 = m23 * m12 - m22 * m13;

		float d03 = - (v5 * m01 - v4 * m02 + v3 * m03) * rdet;
		float d13 = + (v5 * m00 - v2 * m02 + v1 * m03) * rdet;
		float d23 = - (v4 * m00 - v2 * m01 + v0 * m03) * rdet;
		float d33 = + (v3 * m00 - v1 * m01 + v0 * m02) * rdet;

		return Matrix44(
			d00, d01, d02, d03,
			d10, d11, d12, d13,
			d20, d21, d22, d23,
			d30, d31, d32, d33);
	}

	
	Matrix44 Matrix44::operator*( const Matrix44& rhs ) const
	{		
		Matrix44 res;
		mul(res, *this, rhs);
		return res;
	}
	
	Matrix44& Matrix44::operator*=( const Matrix44& rhs )
	{		
		Matrix44 tmp(*this);
		mul(*this, tmp, rhs);
		return *this;	
	}
	
	float Matrix44::det() const
	{
		float res;

		float a = 
			m(1,1) * (m(2,2) * m(3,3) - m(2,3) * m(3,2)) -
			m(2,1) * (m(1,2) * m(3,3) - m(1,3) * m(3,2)) +
			m(3,1) * (m(1,2) * m(2,3) - m(1,3) * m(2,2));

		float b = 
			m(0,1) * (m(2,2) * m(3,3) - m(2,3) * m(3,2)) -
			m(2,1) * (m(0,2) * m(3,3) - m(0,3) * m(3,2)) +
			m(3,1) * (m(0,2) * m(2,3) - m(0,3) * m(2,2));


		float c = 
			m(0,1) * (m(1,2) * m(3,3) - m(1,3) * m(3,2)) -
			m(1,1) * (m(0,2) * m(3,3) - m(0,3) * m(3,2)) +
			m(3,1) * (m(0,2) * m(1,3) - m(0,3) * m(1,2));

		float d = 
			m(0,1) * (m(1,2) * m(2,3) - m(1,3) * m(2,2)) -
			m(1,1) * (m(0,2) * m(2,3) - m(0,3) * m(2,2)) +
			m(2,1) * (m(0,2) * m(1,3) - m(0,3) * m(1,2));

		res = 
			m(0,0) * a + 
			m(1,0) * b + 
			m(2,0) * c + 
			m(3,0) * d;

		return res;
	}

	Matrix44 Matrix44::multiply33( const Matrix44& mat )
	{
		Matrix44 res;

		mul33(res, *this, mat);

		return res;
	}

}

