#include "UtilCamera.h"
#include "UtilMatrixUtils.h"

namespace Rush
{

	Camera::Camera()
		: m_position(0,0,0)
		, m_x_axis(1,0,0)
		, m_y_axis(0,1,0)
		, m_z_axis(0,0,1)
		, m_aspect(1.0)
		, m_fov(0.78539819f) // quarter-Pi
		, m_clip_near(0)
		, m_clip_far(1)
	{
	}

	Camera::Camera( float aspect, float fov, float clip_near, float clip_far )
		: m_position(0,0,0)
		, m_x_axis(1,0,0)
		, m_y_axis(0,1,0)
		, m_z_axis(0,0,1)
		, m_aspect(aspect)
		, m_fov(fov)
		, m_clip_near(clip_near)
		, m_clip_far(clip_far)
	{
	}

	Camera::~Camera()
	{
	}

	Matrix44 Camera::calculate_view() const
	{
		float x = -m_x_axis.dot(m_position);
		float y = -m_y_axis.dot(m_position);
		float z = -m_z_axis.dot(m_position);

		Matrix44 res(
			m_x_axis.x, m_y_axis.x, m_z_axis.x, 0,
			m_x_axis.y, m_y_axis.y, m_z_axis.y, 0,
			m_x_axis.z, m_y_axis.z, m_z_axis.z, 0,
			x,	y,	z,	1);

		return res;
	}

	Matrix44 Camera::calculate_proj() const
	{
		return MatrixUtils::perspective(m_aspect, m_fov, m_clip_near, m_clip_far);
	}

	void Camera::move( const Vector3& delta )
	{
		m_position += m_x_axis * delta.x;
		m_position += m_y_axis * delta.y;
		m_position += m_z_axis * delta.z;
	}

	void Camera::move_on_axis( float delta, const Vector3& axis )
	{
		m_position += axis * delta;
	}

	void Camera::rotate( const Vector3& delta )
	{
		Matrix44 mx = MatrixUtils::rotation_axis(m_x_axis, delta.x);
		Matrix44 my = MatrixUtils::rotation_axis(m_y_axis, delta.y);
		Matrix44 mz = MatrixUtils::rotation_axis(m_z_axis, delta.z);

		Matrix44 mat(mx * my * mz);

		m_x_axis = mat.transform_vector(m_x_axis);
		m_y_axis = mat.transform_vector(m_y_axis);
		m_z_axis = mat.transform_vector(m_z_axis);

		m_x_axis.normalise_this();
		m_y_axis.normalise_this();
		m_z_axis.normalise_this();
	}

	void Camera::rotate_on_axis( float delta, const Vector3& axis )
	{
		Matrix44 mat = MatrixUtils::rotation_axis(axis, delta);

		m_x_axis = mat.transform_vector(m_x_axis);
		m_y_axis = mat.transform_vector(m_y_axis);
		m_z_axis = mat.transform_vector(m_z_axis);

		m_x_axis.normalise_this();
		m_y_axis.normalise_this();
		m_z_axis.normalise_this();
	}

	void Camera::look_at( const Vector3& position, const Vector3& target, const Vector3& up )
	{
		m_position = position;

		m_z_axis = target-position;
		m_z_axis.normalise_this();

		m_x_axis = up.normalised().cross(m_z_axis);
		m_y_axis = m_z_axis.cross(m_x_axis);
		m_x_axis = m_y_axis.cross(m_z_axis);

		m_x_axis.normalise_this();
		m_y_axis.normalise_this();
		m_z_axis.normalise_this();
	}

	void Camera::set_clip( float near_dist, float far_dist )
	{
		m_clip_near = near_dist;
		m_clip_far  = far_dist;
	}
}
