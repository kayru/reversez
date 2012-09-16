#include "UtilMatrixUtils.h"
#include "MathAABB3.h"

#include <math.h>

namespace Rush
{
	Matrix44 MatrixUtils::translation( const Vector3& v )
	{
		return Matrix44(
			1,   0,   0,   0,
			0,   1,   0,   0,
			0,   0,   1,   0,
			v.x, v.y, v.z, 1);
	}

	Matrix44 MatrixUtils::scale( const Vector3& v )
	{
		return Matrix44(
			v.x, 0,   0,   0,
			0,   v.y, 0,   0,
			0,   0,   v.z, 0,
			0,   0,   0,   1);
	}

	Matrix44 MatrixUtils::orthographic( float w, float h, float zn /*= 1*/, float zf /*= -1*/ )
	{
		return Matrix44(
			2.0f/w, 0,		0,				0,
			0,		2.0f/h, 0,				0,
			0,		0,		1.0f/(zf-zn),	0,
			0,		0,		zn/(zn-zf),		1.0f);
	}

	Matrix44 MatrixUtils::orthographic_off_center( float l, float r, float b, float t, float zn, float zf )
	{
		return Matrix44(
			2.0f/(r-l),		0,				0,				0,
			0,				2.0f/(t-b),		0,				0,
			0,				0,				1.0f/(zf-zn),	0,
			(l+r)/(l-r),	(t+b)/(b-t),	zn/(zn-zf),		1);
	}

	Matrix44 MatrixUtils::orthographic_off_center( AABB3& bounds )
	{
		return orthographic_off_center(	
			bounds.vmin().x, bounds.vmax().x,
			bounds.vmin().y, bounds.vmax().y,
			bounds.vmin().z, bounds.vmax().z);
	}

	Matrix44 MatrixUtils::perspective( float aspect, float fov, float clip_near, float clip_far )
	{
		float sy = 1.0f / tan(fov*0.5f);
		float sx = sy / aspect;

		float a = clip_far / (clip_far-clip_near);
		float b = -clip_near*clip_far / (clip_far-clip_near);

		Matrix44 res(
			sx, 0,  0, 0,
			0,  sy, 0, 0, 
			0,  0,  a, 1,
			0,  0,  b, 0);

		return res;
	}


	Matrix44 MatrixUtils::scale_translation( const Vector3& s, const Vector3& t )
	{
		return Matrix44(
			s.x, 0,   0,   0,
			0,   s.y, 0,   0,
			0,   0,   s.z, 0,
			t.x, t.y, t.z, 1);
	}

	Matrix44 MatrixUtils::rotation_x(float angle)
	{		
		float sin_a = sin(angle);
		float cos_a = cos(angle);

		Matrix44 res(
			1, 0,		0,		0,
			0,	cos_a,	sin_a,	0,
			0,	-sin_a,	cos_a,	0,
			0,	0,		0,		1
			);

		return res;
	}

	Matrix44 MatrixUtils::rotation_y(float angle)
	{
		float sin_a = sin(angle);
		float cos_a = cos(angle);

		Matrix44 res(
			cos_a,	0,	-sin_a,	0,
			0,		1,	0,		0,
			sin_a,	0,	cos_a,	0,
			0,		0,	0,		1
			);

		return res;
	}

	Matrix44 MatrixUtils::rotation_z( float angle )
	{
		float sin_a = sin(angle);
		float cos_a = cos(angle);

		Matrix44 res(
			cos_a,	sin_a,	0,	0,
			-sin_a,	cos_a,	0,	0,
			0,		0,		1,	0,
			0,		0,		0,	1);

		return res;
	}

	Matrix44 MatrixUtils::rotation_z( const Vector3& dir )
	{
		Matrix44 res(
			dir.z,	0,	-dir.x,	0,
			0,		1,	0.0f,	0,
			dir.x,	0,	dir.z,	0,
			0,		0,	0,		1);

		return res;
	}

	Matrix44 MatrixUtils::rotation_axis( const Vector3& axis, float angle )
	{
		Matrix44 res;

		float sin_a = sin(angle);
		float cos_a = cos(angle);

		float omc = 1.0f - cos_a;

		float xomc = axis.x * omc;
		float yomc = axis.y * omc;
		float zomc = axis.z * omc;

		float xxomc = axis.x * xomc;
		float xyomc = axis.x * yomc;
		float xzomc = axis.x * zomc;
		float yyomc = axis.y * yomc;
		float yzomc = axis.y * zomc;
		float zzomc = axis.z * zomc;

		float xs = axis.x * sin_a;
		float ys = axis.y * sin_a;
		float zs = axis.z * sin_a;

		res.m(0,0) = xxomc + cos_a;
		res.m(0,1) = xyomc + zs;
		res.m(0,2) = xzomc - ys;
		res.m(0,3) = 0;

		res.m(1,0) = xyomc - zs;
		res.m(1,1) = yyomc + cos_a;
		res.m(1,2) = yzomc + xs;
		res.m(1,3) = 0;

		res.m(2,0) = xzomc + ys;
		res.m(2,1) = yzomc - xs;
		res.m(2,2) = zzomc + cos_a;
		res.m(2,3) = 0; //-V525

		res.m(3,0) = 0;
		res.m(3,1) = 0;
		res.m(3,2) = 0;
		res.m(3,3) = 1;

		return res;
	}
}
