#pragma once

#include "MathCommon.h"
#include "MathMatrix44.h"

namespace Rush
{
	/// @addtogroup util
	/// @{

	class Camera
	{

	public:

		Camera();
		Camera(float aspect, float fov, float clip_near, float clip_far);
		~Camera();

		void set_aspect(float aspect) { m_aspect = aspect; }
		void set_fov(float fov) { m_fov = fov; }
		void set_clip(float near, float far);

		void move(const Vector3& delta);
		void move_on_axis(float delta, const Vector3& axis);
		void rotate(const Vector3& delta);
		void rotate_on_axis(float delta, const Vector3& axis);

		void look_at(const Vector3& position, const Vector3& target, const Vector3& up=Vector3(0,1,0));

		INLINE float near_plane() const { return m_clip_near; }
		INLINE float far_plane() const { return m_clip_far; }

		INLINE float aspect() const { return m_aspect; }
		INLINE float fov() const { return m_fov; }

		INLINE const Vector3& position() const { return m_position; }
		INLINE Vector3& position() { return m_position; }

		Matrix44 calculate_view() const;
		Matrix44 calculate_proj() const;

		INLINE const Vector3& right()		const	{ return m_x_axis; }
		INLINE const Vector3& up()			const	{ return m_y_axis; }
		INLINE const Vector3& direction()	const	{ return m_z_axis; }

	private:

		Vector3	m_position;

		Vector3 m_x_axis; // right
		Vector3	m_y_axis; // up
		Vector3	m_z_axis; // front

		float	m_aspect;
		float	m_fov;
		float	m_clip_near;
		float	m_clip_far;

	};

	/// @}

}


