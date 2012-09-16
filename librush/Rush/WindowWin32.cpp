#include "Rush.h"

#ifdef RUSH_PLATFORM_WINDOWS

#include "WindowWin32.h"
#include "WindowEvents.h"
#include "Platform.h"

#include <stdio.h>
#include <tchar.h>

#ifndef WM_NCMOUSEHOVER
#define WM_NCMOUSEHOVER 0x02A0
#endif //WM_NCMOUSEHOVER

#ifndef WM_NCMOUSELEAVE
#define WM_NCMOUSELEAVE 0x02A2
#endif //WM_NCMOUSELEAVE

namespace Rush
{
	Win32Window::Win32Window(uint32 width, uint32 height, bool resizable, const char* name, RushPlatformContext* context)
		: Window(width, height)
		, m_hwnd(0)
		, m_max_size(4096, 4096)
		, m_pending_size(m_size)
		, m_pos(0,0)
		, m_maximized(false)
		, m_minimized(false)
		, m_resizing(false)
        , m_context(context)
	{
		// register window class
		
		WNDCLASSEX wc = {0};

		HINSTANCE hInst = GetModuleHandle(NULL);

		//WNDPROC;

		wc.cbSize			= sizeof(WNDCLASSEX);
		wc.style			= CS_DBLCLKS | CS_OWNDC | CS_HREDRAW |CS_VREDRAW;
		wc.lpfnWndProc		= wnd_proc;
		wc.cbClsExtra		= 0;
		wc.cbWndExtra		= 0;
		wc.hInstance		= hInst;
		wc.hIcon			= LoadIcon (NULL, IDI_APPLICATION);
		wc.hCursor			= LoadCursor (NULL, IDC_ARROW);
		wc.hbrBackground	= (HBRUSH) GetStockObject (WHITE_BRUSH);
		wc.lpszMenuName		= NULL;
		wc.lpszClassName	= _T("RushWin32Window");
		wc.hIconSm			= wc.hIcon;

		RegisterClassEx(&wc);

		uint32	window_style = WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
		

		if( resizable ) window_style |= WS_SIZEBOX|WS_MAXIMIZEBOX;

		// find screen center and create our window there

		int32	pos_x = GetSystemMetrics(SM_CXSCREEN) / 2 - m_size.x/2;
		int32	pos_y = GetSystemMetrics(SM_CYSCREEN) / 2 - m_size.y/2;

		// calculate window size for required client area

		RECT client_rect = { pos_x, pos_y, pos_x+m_size.x, pos_y+m_size.y};
		AdjustWindowRect(&client_rect, window_style, FALSE);

		// create window

		m_hwnd = CreateWindowA
		(
			"RushWin32Window",
			name,
			window_style,
			client_rect.left, 
			client_rect.top, 
			client_rect.right-client_rect.left, 
			client_rect.bottom-client_rect.top, 
			NULL, 
			NULL, 
			hInst, 
			NULL
		); 

		// setup window owner for message handling

		SetWindowLongPtr(m_hwnd, GWLP_USERDATA, (LONG_PTR)this);

		ShowWindow(m_hwnd, SW_SHOWNORMAL);
		UpdateWindow(m_hwnd);
	}

	Win32Window::~Win32Window()
	{
		
	}	

	LRESULT APIENTRY Win32Window::wnd_proc( HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam )
	{
		Win32Window* window = reinterpret_cast<Win32Window*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));		

		if( window )
		{
			window->process_message(hwnd, msg, wparam, lparam);
		}
		
		return (LRESULT)DefWindowProc(hwnd, msg, wparam, lparam);
	}

	void* Win32Window::native_window_handle()
	{
		return (void*)&m_hwnd;
	}

	void Win32Window::set_caption( const Rush::String& str )
	{
		m_caption = str;
		SetWindowTextA(m_hwnd, str.data());
	}

	bool Win32Window::process_message( HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam )
	{
		
		if( m_interceptor )
		{
			MSG m;
			m.hwnd = hwnd;
			m.lParam = lparam;
			m.wParam = wparam;
			m.message = msg;
			bool message_handled = m_interceptor->process_intercepted_message(&m);
			if( message_handled )
			{
				return true;
			}
		}

		switch(msg)
		{
		case WM_MOUSEMOVE:
		case WM_LBUTTONDOWN:
		case WM_RBUTTONDOWN:
		case WM_MBUTTONDOWN:
		case WM_LBUTTONUP:
		case WM_RBUTTONUP:
		case WM_MBUTTONUP:
		case WM_LBUTTONDBLCLK:
		case WM_RBUTTONDBLCLK:
		case WM_MBUTTONDBLCLK:
		case WM_NCMOUSELEAVE:
		case WM_MOUSEWHEEL:
		case WM_MOUSEHWHEEL:
			process_mouse_event(msg, wparam, lparam);
			return true;

		case WM_KEYDOWN:
		case WM_KEYUP:
			process_key_event(msg, wparam, lparam);
			return true;

		case WM_SIZE:
			process_size_event(wparam, lparam);
			return true;

		case WM_ENTERSIZEMOVE:
			m_resizing = true;
			break;

		case WM_EXITSIZEMOVE:
			m_resizing = false;
			finish_resizing();
			break;
			
		case WM_CLOSE:
			close();
			return true;
		}

		return false;
	}

	void Win32Window::process_mouse_event( UINT message, WPARAM wparam, LPARAM lparam )
	{				
		if( message != WM_NCMOUSELEAVE)
		{
			int32 xPos = LOWORD(lparam); 
			int32 yPos = HIWORD(lparam); 

			/*
			Vector2 mouse_pos(
				(float)xPos / float(m_size.x), 
				(float)yPos / float(m_size.y) );

			mouse_pos.y	 = 1.0f-mouse_pos.y;
			mouse_pos	*= 2;
			mouse_pos	-= 1.0f;
			*/

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

		switch(message)
		{
		case WM_MOUSEMOVE:
			listeners().broadcast(WindowEvent::MouseMove(m_mouse.pos));
            RushPlatform_MouseMove(m_context, m_mouse.pos.x, m_mouse.pos.y);
			break;

		case WM_LBUTTONDOWN:
			m_keyboard.keys[VK_LBUTTON] = true;
			m_mouse.buttons[0] = true;
			listeners().broadcast(WindowEvent::MouseDown(m_mouse.pos, 0, false));
            RushPlatform_MouseDown(m_context, 0, m_mouse.pos.x, m_mouse.pos.y);
			break;

		case WM_LBUTTONUP:
			m_keyboard.keys[VK_LBUTTON] = false;
			m_mouse.buttons[0] = false;
			listeners().broadcast(WindowEvent::MouseUp(m_mouse.pos, 0));
            RushPlatform_MouseUp(m_context, 0, m_mouse.pos.x, m_mouse.pos.y);
			break;

		case WM_LBUTTONDBLCLK:
			m_keyboard.keys[VK_LBUTTON] = true;
			m_mouse.buttons[0] = true;
			listeners().broadcast(WindowEvent::MouseDown(m_mouse.pos, 0, true));
            RushPlatform_MouseDown(m_context, 0, m_mouse.pos.x, m_mouse.pos.y);
			break;

		case WM_RBUTTONDOWN:
			m_keyboard.keys[VK_RBUTTON] = true;
			m_mouse.buttons[1] = true;
			listeners().broadcast(WindowEvent::MouseDown(m_mouse.pos, 1, false));
            RushPlatform_MouseDown(m_context, 1, m_mouse.pos.x, m_mouse.pos.y);
			break;

		case WM_RBUTTONUP:
			m_keyboard.keys[VK_RBUTTON] = false;
			m_mouse.buttons[1] = false;
			listeners().broadcast(WindowEvent::MouseUp(m_mouse.pos, 1));
            RushPlatform_MouseUp(m_context, 1, m_mouse.pos.x, m_mouse.pos.y);
			break;

		case WM_RBUTTONDBLCLK:
			m_keyboard.keys[VK_RBUTTON] = true;
			m_mouse.buttons[1] = true;
			listeners().broadcast(WindowEvent::MouseDown(m_mouse.pos, 1, true));
            RushPlatform_MouseDown(m_context, 1, m_mouse.pos.x, m_mouse.pos.y);
			break;

		case WM_MBUTTONDOWN:
			m_keyboard.keys[VK_MBUTTON] = true;
			m_mouse.buttons[2] = true;
			listeners().broadcast(WindowEvent::MouseDown(m_mouse.pos, 2, false));
            RushPlatform_MouseDown(m_context, 2, m_mouse.pos.x, m_mouse.pos.y);
			break;

		case WM_MBUTTONUP:
			m_keyboard.keys[VK_MBUTTON] = false;
			m_mouse.buttons[2] = false;
			listeners().broadcast(WindowEvent::MouseUp(m_mouse.pos, 2));
            RushPlatform_MouseUp(m_context, 2, m_mouse.pos.x, m_mouse.pos.y);
			break;

		case WM_MBUTTONDBLCLK:
			m_keyboard.keys[VK_MBUTTON] = true;
			m_mouse.buttons[2] = true;
			listeners().broadcast(WindowEvent::MouseDown(m_mouse.pos, 2, true));
            RushPlatform_MouseDown(m_context, 2, m_mouse.pos.x, m_mouse.pos.y);
			break;

		case WM_MOUSEHWHEEL:
			m_mouse.wheel_h += (int32)GET_WHEEL_DELTA_WPARAM(wparam);
            RushPlatform_MouseWheel(m_context, m_mouse.wheel_h, m_mouse.wheel_v);
			break;

		case WM_MOUSEWHEEL:
			m_mouse.wheel_v += (int32)GET_WHEEL_DELTA_WPARAM(wparam);
            RushPlatform_MouseWheel(m_context, m_mouse.wheel_h, m_mouse.wheel_v);
			break;

		}

	}

	void Win32Window::process_key_event( UINT message, WPARAM wparam, LPARAM lparam )
	{		
		R_UNUSED(wparam);
		R_UNUSED(lparam); 

		uint8 code = uint8(wparam);

		switch( message )
		{
		case WM_KEYDOWN:
			m_keyboard.keys[code] = true;
			listeners().broadcast(WindowEvent::KeyDown(code));
            RushPlatform_KeyDown(m_context, code);
			break;

		case WM_KEYUP:
			m_keyboard.keys[code] = false;
			listeners().broadcast(WindowEvent::KeyUp(code));
            RushPlatform_KeyUp(m_context, code);
			break;

		case WM_CHAR:
			//TODO
			break;
		}

	}

	void Win32Window::finish_resizing()
	{
		if( m_size != m_pending_size )
		{
			m_size = m_pending_size;
			listeners().broadcast(WindowEvent::Resize(m_size.x, m_size.y));
		}
	}

	void Win32Window::process_size_event( WPARAM wparam, LPARAM lparam )
	{
		m_pending_size.x = LOWORD(lparam);
		m_pending_size.y = HIWORD(lparam);

		if( wparam == SIZE_MINIMIZED )
		{
			m_maximized = false;
			m_minimized = true;
		}
		if( wparam == SIZE_MAXIMIZED )
		{
			m_maximized = true;
			m_minimized = false;
			finish_resizing();
		}
		if( wparam == SIZE_RESTORED )
		{			
			m_minimized = false;
			m_maximized = false;

			if( m_resizing == false )
			{
				finish_resizing();
			}
		}
	}

	void Win32Window::set_size( const Point2& size )
	{
		R_UNUSED(size);
		//todo
	}

}

#else //RUSH_PLATFORM_WINDOWS

char win32_window_cpp_199919;

#endif //RUSH_PLATFORM_WINDOWS
