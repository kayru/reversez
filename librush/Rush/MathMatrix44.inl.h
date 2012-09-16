#pragma once

namespace Rush
{
	Matrix44::Matrix44(
		float m00, float m10, float m20, float m30,
		float m01, float m11, float m21, float m31,
		float m02, float m12, float m22, float m32,
		float m03, float m13, float m23, float m33 )
	{
		rows[0] = Vector4(m00, m10, m20, m30);
		rows[1] = Vector4(m01, m11, m21, m31);
		rows[2] = Vector4(m02, m12, m22, m32);
		rows[3] = Vector4(m03, m13, m23, m33);
	}

	Matrix44::Matrix44(
		float m00, float m10, float m20,
		float m01, float m11, float m21,
		float m02, float m12, float m22)
	{
		rows[0] = Vector4(m00, m10, m20, 0);
		rows[1] = Vector4(m01, m11, m21, 0);
		rows[2] = Vector4(m02, m12, m22, 0);
		rows[3] = Vector4(0,   0,   0,   1);
	}

	Matrix44::Matrix44( float* arr )
	{
		rows[0].x = arr[0];
		rows[0].y = arr[1];
		rows[0].z = arr[2];
		rows[0].w = arr[3];

		rows[1].x = arr[4];
		rows[1].y = arr[5];
		rows[1].z = arr[6];
		rows[1].w = arr[7];

		rows[2].x = arr[8];
		rows[2].y = arr[9];
		rows[2].z = arr[10];
		rows[2].w = arr[11];

		rows[3].x = arr[12];
		rows[3].y = arr[13];
		rows[3].z = arr[14];
		rows[3].w = arr[15];
	}

    Vector4 Matrix44::column( size_t c ) const
	{
		switch(c)
		{
		case 0:  return Vector4(rows[0].x, rows[1].x, rows[2].x, rows[3].x);
		case 1:  return Vector4(rows[0].y, rows[1].y, rows[2].y, rows[3].y);
		case 2:  return Vector4(rows[0].z, rows[1].z, rows[2].z, rows[3].z);
		case 3:
		default: return Vector4(rows[0].w, rows[1].w, rows[2].w, rows[3].w);
		}
	}

	Vector4& Matrix44::row( size_t r )
	{
		return rows[r];
	}

	const Vector4& Matrix44::row( size_t r ) const
	{
		return rows[r];
	}

	void Matrix44::set_row(size_t r, const Vector4& v)
	{
		rows[r] = v;
	}

	float& Matrix44::operator()( size_t r, size_t c )
	{
		return rows[r].array()[c];
	}

	const float& Matrix44::operator()( size_t r, size_t c ) const
	{
		return rows[r].array()[c];
	}

	float& Matrix44::m( size_t r, size_t c )
	{
		return rows[r].array()[c];
	}

	const float& Matrix44::m( size_t r, size_t c ) const
	{
		return rows[r].array()[c];
	}

	Matrix44 Matrix44::transposed() const
	{
		Matrix44 res;

		res.m(0,0) = m(0,0);
		res.m(0,1) = m(1,0);
		res.m(0,2) = m(2,0);
		res.m(0,3) = m(3,0);

		res.m(1,0) = m(0,1);
		res.m(1,1) = m(1,1);
		res.m(1,2) = m(2,1);
		res.m(1,3) = m(3,1);

		res.m(2,0) = m(0,2);
		res.m(2,1) = m(1,2);
		res.m(2,2) = m(2,2);
		res.m(2,3) = m(3,2);

		res.m(3,0) = m(0,3);
		res.m(3,1) = m(1,3);
		res.m(3,2) = m(2,3);
		res.m(3,3) = m(3,3);

		return res;
	}

	void Matrix44::transpose()
	{
		Matrix44 res;

		res.m(0,0) = m(0,0);
		res.m(0,1) = m(1,0);
		res.m(0,2) = m(2,0);
		res.m(0,3) = m(3,0);

		res.m(1,0) = m(0,1);
		res.m(1,1) = m(1,1);
		res.m(1,2) = m(2,1);
		res.m(1,3) = m(3,1);

		res.m(2,0) = m(0,2);
		res.m(2,1) = m(1,2);
		res.m(2,2) = m(2,2);
		res.m(2,3) = m(3,2);

		res.m(3,0) = m(0,3);
		res.m(3,1) = m(1,3);
		res.m(3,2) = m(2,3);
		res.m(3,3) = m(3,3);

		*this = res;
	}

	Vector4 Matrix44::transform_vector( const Vector4& v ) const
	{
		Vector4 res;

		res.x = v.x*m(0,0) + v.y*m(1,0) + v.z*m(2,0) + v.w*m(3,0);
		res.y = v.x*m(0,1) + v.y*m(1,1) + v.z*m(2,1) + v.w*m(3,1);
		res.z = v.x*m(0,2) + v.y*m(1,2) + v.z*m(2,2) + v.w*m(3,2);
		res.w = v.x*m(0,3) + v.y*m(1,3) + v.z*m(2,3) + v.w*m(3,3);

		return res;
	}

	Vector3 Matrix44::transform_vector( const Vector3& v3 ) const
	{
		Vector4 v(v3.x, v3.y, v3.z, 1.0f);
		Vector3 res;

		res.x = v.x*m(0,0) + v.y*m(1,0) + v.z*m(2,0) + m(3,0);
		res.y = v.x*m(0,1) + v.y*m(1,1) + v.z*m(2,1) + m(3,1);
		res.z = v.x*m(0,2) + v.y*m(1,2) + v.z*m(2,2) + m(3,2);

		return res;
	}

	Matrix44 Matrix44::zero()
	{
		return Matrix44(
			0, 0, 0, 0,
			0, 0, 0, 0,
			0, 0, 0, 0,
			0, 0, 0, 0 );
	}

	Matrix44 Matrix44::identity()
	{
		return Matrix44(
			1, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1 );
	}
}
