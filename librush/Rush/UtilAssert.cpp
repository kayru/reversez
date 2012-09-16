#include "UtilAssert.h"
#include "UtilLog.h"

#if defined(RUSH_PLATFORM_WINDOWS)

#ifndef NOMINMAX
#define NOMINMAX
#endif//NOMINMAX

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif//WIN32_LEAN_AND_MEAN

#include <windows.h>

#endif//RUSH_PLATFORM_WINDOWS

namespace Rush
{
	MESSAGE_BOX_RESULT R_ERROR_MESSAGE_BOX(const char* msg, const char* caption)
	{
		Log::error(msg);
#ifdef RUSH_PLATFORM_WINDOWS
		int res = MessageBoxA(0, msg, caption, MB_ABORTRETRYIGNORE);
		switch(res)
		{	
		case IDRETRY:
			return MESSAGE_BOX_RESULT_RETRY;
			break;
		case IDABORT:
			return MESSAGE_BOX_RESULT_ABORT;
			break;
		case IDIGNORE:
			return MESSAGE_BOX_RESULT_IGNORE;
			break;
		}
#else //RUSH_PLATFORM_WINDOWS
		R_UNUSED(caption);
#endif //RUSH_PLATFORM_WINDOWS
		return MESSAGE_BOX_RESULT_OK;
	}

	void R_TERMINATE_PROCESS()
	{    
#ifdef RUSH_PLATFORM_WINDOWS
		TerminateProcess(GetCurrentProcess(), 1);
#else // RUSH_PLATFORM_WINDOWS
        //TODO
#endif // RUSH_PLATFORM_WINDOWS
	}

}
