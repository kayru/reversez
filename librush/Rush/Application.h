#pragma once

#if 0

#include "SystemPlatform.h"
#include "SystemWindow.h"
#include "SystemWindowEvents.h"

namespace Rush
{
	class RenderDevice;
	class PrimitiveBatch;

    // TODO: App class needs to be deprecated!
    // Some platforms (consoles, mobiles) dont have a windowing system.
    // Can't have a one-fits-all "Application" class.

	class App
	{
	public:

		App(const AppConfig& cfg);
		virtual ~App();

		Window*			window()		{ return m_window; }
		const Window*	window() const	{ return m_window; }

		RenderDevice*	device()		{ return m_device; }

		const AppConfig& cfg() const	{ return m_cfg; }

		//////////////////////////////////////////////////////////////////////////
		// Application virtual interface

		virtual bool tick() {return false;}
		virtual void key_down(uint8 /*code*/) {};

		//////////////////////////////////////////////////////////////////////////

		bool platform_tick();

	private:

		App(const App&);
		App& operator =(const App&);

	private:

		AppConfig						m_cfg;
		Window*							m_window;
		WindowEvent::KeyDownListener	m_keydown;
		RenderDevice*					m_device;

	};

}

#endif