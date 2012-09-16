#pragma once

#include "Rush.h"
#include "MathMatrix44.h"

namespace Rush
{
	/// @addtogroup util
	/// @{

	class AABB3;

	struct MatrixUtils
	{
		static Matrix44 translation(const Vector3& v);
		static inline Matrix44	translation(float x, float y, float z)
		{
			return translation(Vector3(x,y,z));
		}

		static Matrix44 scale(const Vector3& v);
		static inline Matrix44 scale(float x, float y, float z) 
		{
			return scale(Vector3(x,y,z)); 
		}

		static Matrix44	scale_translation(const Vector3& s, const Vector3& t);
		static inline Matrix44 scale_translation(float sx, float sy, float sz, float tx, float ty, float tz) 
		{
			return scale_translation(Vector3(sx,sy,sz), Vector3(tx,ty,tz)); 
		}

		static Matrix44 rotation_x(float angle);
		static Matrix44 rotation_y(float angle);
		static Matrix44 rotation_z(float angle);
		static Matrix44 rotation_z(const Vector3& dir);

		static Matrix44 rotation_axis(const Vector3& axis, float angle);

		static Matrix44 look_at(const Vector3& position, const Vector3& target, const Vector3& up);
		static Matrix44 look_at(const Vector3& dir, const Vector3& up);

		static Matrix44 orthographic(float w, float h, float zn = 1, float zf = -1);
		static Matrix44 orthographic_off_center(float l, float r, float b, float t, float zn, float zf);
		static Matrix44 orthographic_off_center(AABB3& bounds);
		static Matrix44 perspective(float aspect, float fov, float clip_near, float clip_far);
	};

	/// @}
}


