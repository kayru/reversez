#include "Application.h"

#if 0
#include "SystemPlatform.h"
#include "SystemWindow.h"
#include "UtilAssert.h"
#include "RenderDevice.h"

namespace Rush
{

#ifdef RUSH_PLATFORM_IOS
	static Window* RushCreateWindow(uint32, uint32, bool)
	{
		return NULL;
	}
#endif //RUSH_PLATFORM_IOS

	App::App(const AppConfig& cfg)
		: m_cfg(cfg)
		, m_window(NULL)
	{

		m_window = RushCreateWindow(m_cfg.width, m_cfg.height, m_cfg.resizable);

		if( m_window )
		{
			m_window->set_caption(m_cfg.name);
			m_keydown.set_owner(m_window);
		}

		m_device = new RenderDevice(m_window);
	}

	App::~App()
	{
		delete m_device;
		m_keydown.set_owner(NULL);
		delete m_window;
	}

	bool App::platform_tick()
	{
		for(uint32 i=0; i<m_keydown.size();++i )
		{
			key_down(m_keydown[i].code);
		}
		m_keydown.clear();

		if( window()->is_closed() )
		{
			return false;
		}
		else
		{
			return tick();
		}
	}
}

#endif