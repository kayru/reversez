#include "UtilTimer.h"

#ifndef RUSH_EXTERN_PLATFORM
#ifdef RUSH_PLATFORM_WINDOWS

#ifndef NOMINMAX
#define NOMINMAX
#endif//NOMINMAX

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif//WIN32_LEAN_AND_MEAN

#include <windows.h>

#else //RUSH_PLATFORM_WINDOWS
#include <sys/time.h>
#endif //RUSH_PLATFORM_WINDOWS

namespace Rush
{

	Timer::Timer(void)
	{
#if defined(RUSH_PLATFORM_WINDOWS)
		QueryPerformanceFrequency((LARGE_INTEGER*)&freq);
		QueryPerformanceCounter((LARGE_INTEGER*)&t);
#endif // RUSH_PLATFORM_WINDOWS
	}

	Timer::~Timer(void)
	{
	}


	void Timer::reset()
	{
#if defined(RUSH_PLATFORM_WINDOWS)
		QueryPerformanceCounter((LARGE_INTEGER*)&t);
#endif // RUSH_PLATFORM_WINDOWS
	}
	
	float Timer::time() const
	{
#if defined(RUSH_PLATFORM_WINDOWS)
		uint64 curtime;
		uint64 elapsed;

		QueryPerformanceCounter((LARGE_INTEGER*)&curtime);

		elapsed = curtime - t;

		return elapsed / (float)freq;
#else 
        return 0.0; // TODO
#endif // defined(RUSH_PLATFORM_WINDOWS)
		
	
	}

	void Timer::sleep(uint32 ms)
	{
#if defined(RUSH_PLATFORM_WINDOWS)
		Sleep(ms);
#else //RUSH_PLATFORM_WINDOWS
		//nanosleep(1000000*ms); // TODO
#endif //RUSH_PLATFORM_WINDOWS
	}

}
#endif //RUSH_EXTERN_PLATFORM
