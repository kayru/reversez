#pragma once

#include "Rush.h"
#include "MathVector2.h"

struct RushPlatformContext;

namespace Rush
{
	/// @addtogroup util
	/// @{

	class Camera;

	class CameraManipulator
	{
	public: 

		enum KeyFunction
		{
			KeyFunction_MoveX_Pos,
			KeyFunction_MoveX_Neg,
			KeyFunction_MoveY_Pos,
			KeyFunction_MoveY_Neg,
			KeyFunction_MoveZ_Pos,
			KeyFunction_MoveZ_Neg,

			KeyFunction_RotateX_Pos,
			KeyFunction_RotateX_Neg,
			KeyFunction_RotateY_Pos,
			KeyFunction_RotateY_Neg,

			KeyFunction_Faster,
			KeyFunction_Slower,

			KeyFunction_COUNT
		};

		enum Mode
		{
			Mode_FPS,
			Mode_Orbit
		};

		CameraManipulator();
		CameraManipulator(Camera* camera, RushPlatformContext* context);
		~CameraManipulator();

		void init(Camera* camera, RushPlatformContext* context);

		void step(float dt);

		void set_move_speed(float speed) { m_move_speed = speed; }
		void set_turn_speed(float speed) { m_turn_speed = speed; }

		void set_mode(Mode mode) { m_mode = mode; }
		Mode get_mode() const { return m_mode; }

		void  set_default_keys();
		void  set_key(KeyFunction fun, uint8 key);
		uint8 get_key(KeyFunction fun);

	private:

		void pr_step_fps(float dt, const Vector2& mouse_delta, int32 wheel_delta);
		void pr_step_orbit(float dt, const Vector2& mouse_delta, int32 wheel_delta);

	private:

		Camera*                 m_camera;
		RushPlatformContext*    m_context;

		Vector2                 m_old_mouse_pos;
		int32                   m_old_mouse_wheel;

		float                   m_move_speed;
		float                   m_turn_speed;

		Mode                    m_mode;
		uint8                   m_keys[KeyFunction_COUNT];

	};

	/// @}

}

