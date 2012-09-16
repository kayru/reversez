#pragma once

#include "Rush.h"

namespace Rush
{
	/// @addtogroup util
	/// @{

	enum MESSAGE_BOX_RESULT
	{
		MESSAGE_BOX_RESULT_OK,
		MESSAGE_BOX_RESULT_CANCEL,
		MESSAGE_BOX_RESULT_RETRY,
		MESSAGE_BOX_RESULT_ABORT,
		MESSAGE_BOX_RESULT_IGNORE
	};

	MESSAGE_BOX_RESULT R_ERROR_MESSAGE_BOX(const char* msg, const char* caption="Message");
	void R_TERMINATE_PROCESS();

#if defined(_DEBUG)||defined(FORCE_ASSERTS)
	#define R_STRINGIFY_FOR_ASSERTS(x) #x
	#define R_TOSTRING_FOR_ASSERTS(x) R_STRINGIFY_FOR_ASSERTS(x)
	#define R_ASSERT(v)\
	{\
		if(!(v))\
		{\
			MESSAGE_BOX_RESULT res = R_ERROR_MESSAGE_BOX(__FILE__"("R_TOSTRING_FOR_ASSERTS(__LINE__)")\nFunction: "R_FUNCTION"\nExpression: "#v, "Assert");\
			switch(res)\
			{\
			case MESSAGE_BOX_RESULT_RETRY:\
				R_BREAK;\
				break;\
			case MESSAGE_BOX_RESULT_ABORT:\
				R_TERMINATE_PROCESS();\
				break;\
            case MESSAGE_BOX_RESULT_IGNORE:\
            case MESSAGE_BOX_RESULT_OK:\
            case MESSAGE_BOX_RESULT_CANCEL:\
				break;\
			}\
		}\
	}
	#define R_ASSERT_MSG(v,msg)\
	{\
		if(!(v))\
		{\
			MESSAGE_BOX_RESULT res = R_ERROR_MESSAGE_BOX(__FILE__"("R_TOSTRING_FOR_ASSERTS(__LINE__)")\nFunction: "R_FUNCTION"\nExpression: "#v"\nMessage: "msg, "Assert");\
			switch(res)\
			{\
			case MESSAGE_BOX_RESULT_RETRY:\
				R_BREAK;\
				break;\
			case MESSAGE_BOX_RESULT_ABORT:\
				R_TERMINATE_PROCESS();\
				break;\
			case MESSAGE_BOX_RESULT_IGNORE:\
            case MESSAGE_BOX_RESULT_OK:\
            case MESSAGE_BOX_RESULT_CANCEL:\
				break;\
			}\
		}\
	}
	#define R_ERROR R_BREAK
#else//_DEBUG
	#define R_ASSERT(v) R_UNUSED((v))
	#define R_ASSERT_MSG(v,msg)
	#define R_ERROR
#endif //_DEBUG

	/// @}

}
