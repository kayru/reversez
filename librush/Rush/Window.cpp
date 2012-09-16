#include "Window.h"
#include "WindowEvents.h"

namespace Rush
{
	Window::Window(uint32 width, uint32 height)
		: m_size(width,height)
		, m_pos(0,0)
		, m_closed(false)
		, m_listeners(new WindowEvent::ListenerCollection())
		, m_interceptor(NULL)
	{

	}

	Window::~Window()
	{
		delete m_listeners;
	}
	
}
