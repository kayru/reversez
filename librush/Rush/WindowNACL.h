#pragma once

#ifdef RUSH_PLATFORM_NACL

#include "UtilString.h"
#include "SystemWindow.h"

namespace pp
{
	class MouseInputEvent;
	class KeyboardInputEvent;
}

namespace Rush
{
	class NACLWindow : public Window
	{

	public:
		
		NACLWindow(uint32 width, uint32 height, bool resizable);
		virtual ~NACLWindow();

		void process_mouse_event(const pp::MouseInputEvent& ev);
		void process_key_event(const pp::KeyboardInputEvent& ev);

		//////////////////////////////////////////////////////////////////////////
		// virtual interfaces (Window)

		virtual void*	native_window_handle();
		virtual void	set_caption(const Rush::String& str);
		virtual void	set_size(const Rush::Point2& size);

	private:
		
		Rush::String		m_caption;

		Rush::Point2		m_max_size;
		Rush::Point2		m_pending_size;

		Rush::Point2		m_pos;

		bool				m_maximized;
		bool				m_minimized;
		bool				m_resizing;

	};
}

#endif //RUSH_PLATFORM_NACL
