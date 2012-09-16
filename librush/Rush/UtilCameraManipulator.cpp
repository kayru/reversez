#include "UtilCameraManipulator.h"
#include "UtilCamera.h"
#include "UtilMatrixUtils.h"
#include "Window.h"
#include "Platform.h"

namespace Rush
{
	CameraManipulator::CameraManipulator()
		: m_camera(NULL)
		, m_context(NULL)
		, m_old_mouse_pos(0,0)
		, m_old_mouse_wheel(0)
		, m_move_speed(20.0f)
		, m_turn_speed(5.0f)
		, m_mode(Mode_FPS)
	{
		set_default_keys();
	}

	CameraManipulator::CameraManipulator( Camera* camera, RushPlatformContext* context ) 
		: m_camera(NULL)
		, m_context(NULL)
		, m_old_mouse_pos(0,0)
		, m_old_mouse_wheel(0)
		, m_move_speed(20.0f)
		, m_turn_speed(5.0f)
		, m_mode(Mode_FPS)
	{
		init(camera, context);
		set_default_keys();
	}

	CameraManipulator::~CameraManipulator()
	{

	}

	void CameraManipulator::set_key( KeyFunction fun, uint8 key )
	{
		m_keys[fun] = key;
	}

	uint8 CameraManipulator::get_key( KeyFunction fun )
	{
		return m_keys[fun];
	}

	void CameraManipulator::set_default_keys()
	{
		set_key(KeyFunction_MoveX_Pos, 'D');
		set_key(KeyFunction_MoveX_Neg, 'A');
		set_key(KeyFunction_MoveY_Pos, 'E');
		set_key(KeyFunction_MoveY_Neg, 'Q');
		set_key(KeyFunction_MoveZ_Pos, 'W');
		set_key(KeyFunction_MoveZ_Neg, 'S');

		set_key(KeyFunction_RotateX_Pos, 0x26 /*VK_UP*/);
		set_key(KeyFunction_RotateX_Neg, 0x28 /*VK_DOWN*/);
		set_key(KeyFunction_RotateY_Pos, 0x25 /*VK_LEFT*/);
		set_key(KeyFunction_RotateY_Neg, 0x27 /*VK_LEFT*/);

		set_key(KeyFunction_Faster, 0x10 /*VK_SHIFT*/);
		set_key(KeyFunction_Slower, 0x11 /*VK_CONTROL*/);
	}

	void CameraManipulator::init( Camera* camera, RushPlatformContext* context )
	{
		m_camera = camera;
		m_context = context;
        
        const MouseState& ms = *RushPlatform_GetMouseState(m_context);
        
		m_old_mouse_pos = ms.pos;
		m_old_mouse_wheel = ms.wheel_v;
	}

	void CameraManipulator::step( float dt )
	{
		if( m_context == NULL ) 
		{
			return;
		}
        
		const MouseState& ms = *RushPlatform_GetMouseState(m_context);

		Vector2 mouse_pos = ms.pos;
		Vector2 mouse_delta = mouse_pos - m_old_mouse_pos;
		m_old_mouse_pos = mouse_pos;

		int32 mouse_wheel = ms.wheel_v;
		int32 wheel_delta = mouse_wheel - m_old_mouse_wheel;
		m_old_mouse_wheel = mouse_wheel;

		switch( m_mode )
		{
		case Mode_FPS:
			pr_step_fps(dt, mouse_delta, wheel_delta);
			break;
		case Mode_Orbit:
			pr_step_orbit(dt, mouse_delta, wheel_delta);
			break;
		}

        const RushAppConfig* cfg = RushPlatform_GetConfig(m_context);

		float aspect = float(cfg->width) / float(cfg->height);
		m_camera->set_aspect(aspect);

	}

	void CameraManipulator::pr_step_fps(float dt, const Vector2& mouse_delta, int32 /*wheel_delta*/)
	{
		const KeyboardState& ks = *RushPlatform_GetKeyboardState(m_context);
		const MouseState& ms = *RushPlatform_GetMouseState(m_context);

		Vector3 cam_move(0,0,0);
		Vector3 cam_rotate(0,0,0);

		if( ks.is_key_down(get_key(KeyFunction_MoveX_Pos)) ) cam_move.x =  1;
		if( ks.is_key_down(get_key(KeyFunction_MoveX_Neg)) ) cam_move.x = -1;

		if( ks.is_key_down(get_key(KeyFunction_MoveY_Pos)) ) cam_move.y =  1;
		if( ks.is_key_down(get_key(KeyFunction_MoveY_Neg)) ) cam_move.y = -1;

		if( ks.is_key_down(get_key(KeyFunction_MoveZ_Pos)) ) cam_move.z =  1;
		if( ks.is_key_down(get_key(KeyFunction_MoveZ_Neg)) ) cam_move.z = -1;

		if( ks.is_key_down(get_key(KeyFunction_RotateY_Pos)) ) cam_rotate.y = -2*dt*m_turn_speed;
		if( ks.is_key_down(get_key(KeyFunction_RotateY_Neg)) ) cam_rotate.y =  2*dt*m_turn_speed;

		if( ks.is_key_down(get_key(KeyFunction_RotateX_Pos)) ) cam_rotate.x = -2*dt*m_turn_speed;
		if( ks.is_key_down(get_key(KeyFunction_RotateX_Neg)) ) cam_rotate.x =  2*dt*m_turn_speed;

		if( (ms.buttons[0]) != 0)
		{
			cam_rotate.y = mouse_delta.x*0.005f;
			cam_rotate.x = mouse_delta.y*0.005f;
		}

		if( cam_move!=Vector3(0,0,0) )
		{
			cam_move.normalise_this();
			if( ks.is_key_down(get_key(KeyFunction_Faster)) ) cam_move *= 10.0f;
			if( ks.is_key_down(get_key(KeyFunction_Slower)) ) cam_move *= 0.1f;
			m_camera->move(cam_move*dt*m_move_speed);
		}

		float rot_len = cam_rotate.length();
		if( rot_len>0 )
		{
			m_camera->rotate_on_axis(cam_rotate.y, Vector3(0,1,0));
			m_camera->rotate_on_axis(cam_rotate.x, m_camera->right());
		}
	}

	void CameraManipulator::pr_step_orbit(float dt, const Vector2& mouse_delta, int32 wheel_delta)
	{
		const KeyboardState& ks = *RushPlatform_GetKeyboardState(m_context);
		const MouseState& ms = *RushPlatform_GetMouseState(m_context);

		Vector3 cam_move(0,0,0);

		if( ks.is_key_down(get_key(KeyFunction_MoveX_Pos)) ) cam_move.x =  1;
		if( ks.is_key_down(get_key(KeyFunction_MoveX_Neg)) ) cam_move.x = -1;

		if( ks.is_key_down(get_key(KeyFunction_MoveY_Pos)) ) cam_move.y =  1;
		if( ks.is_key_down(get_key(KeyFunction_MoveX_Neg)) ) cam_move.y = -1;

		if( ks.is_key_down(get_key(KeyFunction_MoveZ_Pos)) ) cam_move.z =  1;
		if( ks.is_key_down(get_key(KeyFunction_MoveZ_Neg)) ) cam_move.z = -1;

		if( ms.buttons[0] != 0 )
		{
			Vector3 old_cam_pos = m_camera->position();
			Vector3 old_cam_dir = m_camera->direction();
			Vector3 old_cam_up = m_camera->up();

			float orbit_radius = 1.0f;
			Vector3 orbit_center = old_cam_pos + old_cam_dir*orbit_radius;

			Camera tmp(1, 1, 1, 1);
			tmp.look_at(-old_cam_dir, Vector3(0,0,0), old_cam_up);
			tmp.rotate_on_axis(mouse_delta.x * 0.01f, Vector3(0,1,0));
			tmp.rotate_on_axis(mouse_delta.y * 0.01f, tmp.right());

			m_camera->look_at(-tmp.direction()*orbit_radius + orbit_center, orbit_center, tmp.up());
		}
		else if( ms.buttons[1] != 0 )
		{
			// move
			Vector2 mp = mouse_delta * 5.0f;
			cam_move.x = mp.x * -1;
			cam_move.y = mp.y;
		}

		if( wheel_delta )
		{
			cam_move.z += float(wheel_delta) * 3.0f;
		}

		if( cam_move!=Vector3(0,0,0) )
		{
			if( ks.is_key_down(get_key(KeyFunction_Faster)) ) cam_move *= 10.0f;
			if( ks.is_key_down(get_key(KeyFunction_Slower)) ) cam_move *= 0.1f;
			m_camera->move(cam_move*dt*m_move_speed);
		}
	}

	

	
}

