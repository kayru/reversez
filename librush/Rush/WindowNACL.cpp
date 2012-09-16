#ifdef RUSH_PLATFORM_NACL

#include "SystemWindowNACL.h"
#include "SystemWindowEvents.h"
#include "UtilLog.h"

#include <ppapi/cpp/input_event.h>
#include <ppapi/cpp/point.h>

Rush::NACLWindow* g_rush_nacl_window = NULL; // temporary hack

namespace Rush
{

	NACLWindow::NACLWindow(uint32 width, uint32 height, bool resizable)
		: Window(width, height)
		, m_max_size(4096, 4096)
		, m_pending_size(m_size)
		, m_pos(0,0)
		, m_maximized(false)
		, m_minimized(false)
		, m_resizing(false)
	{
		R_ASSERT(g_rush_nacl_window == NULL);
		g_rush_nacl_window = this;
	}

	NACLWindow::~NACLWindow()
	{
		g_rush_nacl_window = NULL;
	}

	void* NACLWindow::native_window_handle()
	{
		return NULL;
	}

	void NACLWindow::set_caption( const Rush::String& str )
	{
		m_caption = str;
	}

	void NACLWindow::set_size( const Point2& size )
	{
		R_UNUSED(size);
	}

	void NACLWindow::process_mouse_event( const pp::MouseInputEvent& ev )
	{
		if( ev.GetType() != PP_INPUTEVENT_TYPE_MOUSELEAVE)
		{
			int32 xPos = ev.GetPosition().x();
			int32 yPos = ev.GetPosition().y();

			Vector2 mouse_pos((float)xPos, (float)yPos);

			m_mouse.pos = mouse_pos;
			m_mouse.pos_pixels = Vector2(float(xPos), float(yPos));
		}
		else
		{
			m_mouse.buttons[0] = false;
			m_mouse.buttons[1] = false;
			m_mouse.buttons[2] = false;
		}

		uint8 button_idx  = 0;
		uint8 button_code = 0;

		switch(ev.GetButton())
		{
		case PP_INPUTEVENT_MOUSEBUTTON_LEFT:
			button_idx  = 0;
			button_code = 1;
			break;
		case PP_INPUTEVENT_MOUSEBUTTON_RIGHT:
			button_idx  = 1;
			button_code = 2;
			break;
		case PP_INPUTEVENT_MOUSEBUTTON_MIDDLE:
			button_idx  = 1;
			button_code = 4;
			break;
		}

		if(ev.GetType() == PP_INPUTEVENT_TYPE_MOUSEMOVE)
		{
			listeners().broadcast(WindowEvent::MouseMove(m_mouse.pos));
		}
		else if(ev.GetType() == PP_INPUTEVENT_TYPE_MOUSEDOWN)
		{
			m_keyboard.keys[button_code] = true;
			m_mouse.buttons[button_idx] = true;
			listeners().broadcast(WindowEvent::MouseDown(m_mouse.pos, button_idx, false));
		}
		else if(ev.GetType() == PP_INPUTEVENT_TYPE_MOUSEUP)
		{
			m_keyboard.keys[button_code] = false;
			m_mouse.buttons[button_idx] = false;
			listeners().broadcast(WindowEvent::MouseUp(m_mouse.pos, button_idx));
		}
	}

	void NACLWindow::process_key_event( const pp::KeyboardInputEvent& ev )
	{
		uint8 code = uint8(ev.GetKeyCode());
		switch( ev.GetType() )
		{
		case PP_INPUTEVENT_TYPE_KEYDOWN:
			m_keyboard.keys[code] = true;
			listeners().broadcast(WindowEvent::KeyDown(code));
			break;
		case PP_INPUTEVENT_TYPE_KEYUP:
			m_keyboard.keys[code] = false;
			listeners().broadcast(WindowEvent::KeyUp(code));
			break;
		}
	}

}

#else //RUSH_PLATFORM_NACL

char __nacl_window_cpp_;

#endif //RUSH_PLATFORM_NACL
