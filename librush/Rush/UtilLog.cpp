#include "UtilLog.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>


#ifdef RUSH_PLATFORM_WINDOWS

#ifndef NOMINMAX
#define NOMINMAX
#endif//NOMINMAX

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif//WIN32_LEAN_AND_MEAN

#include <windows.h>

#endif //RUSH_PLATFORM_WINDOWS

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4996) // 'vsnprintf': This function or variable may be unsafe. Consider using vsnprintf_s instead.
#endif //_MSC_VER

namespace Rush
{
	bool Log::break_on_error = false;
	bool Log::break_on_warning = false;

	Log::LogMessageCallback Log::callback_debugger = NULL;
	Log::LogMessageCallback Log::callback_message = NULL;
	Log::LogMessageCallback Log::callback_warning = NULL;
	Log::LogMessageCallback Log::callback_error = NULL;
	Log::LogMessageCallback Log::callback_fatal = NULL;

	void Log::debugger( const char* msg, ... )
	{
		if( callback_debugger )
		{
			char str[1024];
			va_list varargs;
			va_start(varargs,msg);
			vsnprintf(str, sizeof(str), msg, varargs);
			va_end(varargs);
			callback_debugger(str);
		}
		else
		{
			va_list varargs;
			va_start(varargs,msg);

#if defined(_MSC_VER)
			char str[1024];
			vsprintf_s(str,msg,varargs);
			OutputDebugStringA(str);
#else //_MSC_VER
			vfprintf(stdout,msg,varargs);
			fflush(stdout);
#endif //_MSC_VER

			va_end(varargs);
		}

	}

	void Log::message( const char* msg, ...)
	{
		if( callback_message )
		{
			char str[1024];
			va_list varargs;
			va_start(varargs,msg);
			vsnprintf(str, sizeof(str), msg, varargs);
			va_end(varargs);
			callback_message(str);
		}
		else
		{
			va_list varargs;
			va_start(varargs,msg);
			fprintf(stdout, "Message: ");
			vfprintf(stdout,msg,varargs);
			fprintf(stdout, "\n");
			va_end(varargs);
			fflush(stdout);
		}
	}

	void Log::warning( const char* msg, ... )
	{
		if( callback_warning )
		{
			char str[1024];
			va_list varargs;
			va_start(varargs,msg);
			vsnprintf(str, sizeof(str), msg, varargs);
			va_end(varargs);
			callback_warning(str);
		}
		else
		{
			va_list varargs;
			va_start(varargs,msg);
			fprintf(stderr, "Warning: ");
			vfprintf(stderr,msg,varargs);
			fprintf(stderr, "\n");
			va_end(varargs);
			fflush(stderr);
		}

		if( break_on_warning ) R_BREAK;
	}

	void Log::error( const char* msg, ... )
	{
		if( callback_error )
		{
			char str[1024];
			va_list varargs;
			va_start(varargs,msg);
			vsnprintf(str, sizeof(str), msg, varargs);
			va_end(varargs);
			callback_error(str);
		}
		else
		{
			va_list varargs;
			va_start(varargs,msg);
			fprintf(stderr, "Error: ");
			vfprintf(stderr,msg,varargs);
			fprintf(stderr, "\n");
			va_end(varargs);
			fflush(stderr);
		}

		if( break_on_error ) R_BREAK;
	}

	void Log::fatal( const char* msg, ... )
	{
		if( callback_fatal )
		{
			char str[1024];
			va_list varargs;
			va_start(varargs,msg);
			vsnprintf(str, sizeof(str), msg, varargs);
			va_end(varargs);
			callback_fatal(str);
		}
		else
		{
			va_list varargs;
			va_start(varargs,msg);
			fprintf(stderr, "Fatal Error: ");
			vfprintf(stderr,msg,varargs);
			fprintf(stderr, "\n");
			va_end(varargs);
			fflush(stderr);
		}

		if( break_on_error ) R_BREAK;

		exit(EXIT_FAILURE);
	}
	
}

#ifdef _MSC_VER
#pragma warning(pop)
#endif //_MSC_VER
