#include "Rush.h"

#ifdef RUSH_PLATFORM_WINDOWS

#include "Platform.h"
#include "Application.h"
#include "UtilMemory.h"

//////////////////////////////////////////////////////////////////////////


#if 0
int RushMainInner(int /*argc*/, char**  /*argv*/, Rush::App*(*create_app)(const Rush::AppConfig& cfg), const Rush::AppConfig& cfg)
{	
	fix_working_directory();

	Rush::App* app = create_app(cfg);

	for(;;)
	{
		MSG msg;
		while( PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) )
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		if( app->platform_tick() == false) break;
	}
	
	delete app;

	return 0;
}
#endif //0



#else //RUSH_PLATFORM_WINDOWS

char win32_platform_cpp_123331;

#endif //RUSH_PLATFORM_WINDOWS

