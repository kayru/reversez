#ifdef RUSH_PLATFORM_NACL

#include "SystemPlatform.h"
#include "SystemApplication.h"
#include "SystemWindowNACL.h"
#include "UtilMemory.h"

int RushMainInner(int /*argc*/, char**  /*argv*/, Rush::App*(*create_app)(const Rush::AppConfig& cfg), const Rush::AppConfig& cfg)
{	
	// TODO	
	return 0;
}

Rush::Window* RushCreateWindow(unsigned int width, unsigned int height, bool resizable)
{
	return new Rush::NACLWindow(width, height, resizable);
}

#else //RUSH_PLATFORM_NACL

char ___system_platform_nacl_cpp__;

#endif //RUSH_PLATFORM_NACL

