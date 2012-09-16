#pragma once

#ifdef RUSH_PLATFORM_WINDOWS

#ifndef NOMINMAX
#define NOMINMAX
#endif //NOMINMAX

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif //WIN32_LEAN_AND_MEAN

#include <windows.h>

#include "UtilString.h"
#include "Window.h"

struct RushPlatformContext;

namespace Rush
{
	class Win32Window : public Window
	{

	public:
		
		Win32Window(uint32 width, uint32 height, bool resizable, const char* name, RushPlatformContext* context);
		virtual ~Win32Window();

		//////////////////////////////////////////////////////////////////////////
		// virtual interfaces (Window)

		virtual void*	native_window_handle();
		virtual void	set_caption(const Rush::String& str);
		virtual void	set_size(const Rush::Point2& size);

		//////////////////////////////////////////////////////////////////////////
		// non-virtual stuff

		bool process_message(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

		void process_mouse_event(UINT message, WPARAM wparam, LPARAM lparam); 
		void process_key_event(UINT message, WPARAM wparam, LPARAM lparam); 
		void process_size_event(WPARAM wparam, LPARAM lparam); 

	private:

		static LRESULT APIENTRY wnd_proc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

		void finish_resizing();

	private:
		
		HWND				m_hwnd;

		Rush::String		m_caption;

		Rush::Point2		m_max_size;
		Rush::Point2		m_pending_size;

		Rush::Point2		m_pos;

		bool				m_maximized;
		bool				m_minimized;
		bool				m_resizing;

        RushPlatformContext* m_context;
	};
}

#endif //RUSH_PLATFORM_WINDOWS
