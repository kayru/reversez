#ifdef RUSH_PLATFORM_LINUX

#include "SystemPlatform.h"
#include "SystemApplication.h"
#include "UtilMemory.h"

int RushMainInner(int /*argc*/, char**  /*argv*/, Rush::App*(*create_app)(const Rush::AppConfig& cfg), const AppConfig& cfg)
{	
	// TODO	
	return 0;
}

Window* RushCreateWindow(unsigned int width, unsigned int height, bool resizable)
{
	return NULL; // TODO
}

#else //RUSH_PLATFORM_LINUX

char __linux_platform_cpp_20110425__;

#endif //RUSH_PLATFORM_LINUX

