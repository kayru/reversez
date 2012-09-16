#pragma once

#include "Rush.h"

namespace Rush
{
	/// @addtogroup util
	/// @{

	struct Log
	{
		typedef void (*LogMessageCallback)(const char* msg);

#ifdef RUSH_DISABLE_LOGGING
		static INLINE void debugger(const char* , ...){};
		static INLINE void message(const char* , ...){};
		static INLINE void warning(const char* , ...){};
		static INLINE void error(const char* , ...){};
		static INLINE void fatal(const char* , ...){};
#else //RUSH_DISABLE_LOGGING
		static void debugger(const char* msg, ...);
		static void message(const char* msg, ...);
		static void warning(const char* msg, ...);
		static void error(const char* msg, ...);
		static void fatal(const char* msg, ...);
#endif //RUSH_DISABLE_LOGGING

		static bool break_on_warning;
		static bool break_on_error;

		static LogMessageCallback callback_debugger;
		static LogMessageCallback callback_message;
		static LogMessageCallback callback_warning;
		static LogMessageCallback callback_error;
		static LogMessageCallback callback_fatal;
	};

	/// @}

}


