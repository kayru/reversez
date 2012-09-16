#ifndef __RLIB_WINDOW_H
#define __RLIB_WINDOW_H

#include "MathVector2.h"
#include "MathPoint.h"
#include "UtilString.h"

namespace Rush
{
	namespace WindowEvent
	{
		class ListenerCollection;
	}

	class MouseState
	{
	public:

		MouseState()
			: doubleclick(false)
			, pos(0,0)
			, pos_pixels(0,0)
			, wheel_h(0)
			, wheel_v(0)
		{
			rush_memset(buttons, 0, sizeof(buttons));
		}

		bool	buttons[10];
		bool	doubleclick;

		Vector2 pos;
		Vector2 pos_pixels;
		int32 wheel_h;
		int32 wheel_v;
	};

	class KeyboardState
	{
	public:

		KeyboardState()
		{
			rush_memset(keys, 0, sizeof(keys));
		}

		bool is_key_down(uint8 code) const { return keys[code]; }

		bool keys[256];

	};

	class WindowMessageInterceptor
	{
	public:
		virtual ~WindowMessageInterceptor(){}
		virtual bool process_intercepted_message(void* data) = 0;
	};

	class Window
	{

	public:

		Window(uint32 width, uint32 height);
		virtual ~Window();

		//////////////////////////////////////////////////////////////////////////
		// virtual interfaces

		virtual void*	native_window_handle() = 0;
		virtual void	set_caption(const String& str) = 0;
		virtual void	set_size(const Point2& size) = 0;

		//////////////////////////////////////////////////////////////////////////
		// non-virtual interfaces

		void close()			{ m_closed = true; }
		bool is_closed() const	{ return m_closed; }

		uint32 width()	const	{ return m_size.x; }
		uint32 height()	const	{ return m_size.y; }

		float aspect() const { return float(m_size.x) / float(m_size.y); }

		const Point2& size()	const { return m_size; }
		const Point2& pos()		const { return m_pos; }

		const MouseState&		mouse()		const { return m_mouse; }
		const KeyboardState&	keyboard()	const { return m_keyboard; }

		WindowEvent::ListenerCollection& listeners() { return *m_listeners; }

		void set_message_interceptor(WindowMessageInterceptor* ptr) { m_interceptor = ptr; }

	protected:

		Point2			m_size;
		Point2			m_pos;

		bool			m_closed;

		MouseState		m_mouse;
		KeyboardState	m_keyboard;

		WindowEvent::ListenerCollection* m_listeners;

		WindowMessageInterceptor* m_interceptor;
	};
}

#endif //__RLIB_WINDOW_H
