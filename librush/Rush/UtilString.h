#pragma once

#include "Rush.h"
#include "UtilDynamicArray.h"

namespace Rush
{

	/// @addtogroup util
	/// @{

	class String : public DynamicArray<char>
	{
	public:
		
		String();
		~String();

		String(const String& rhs);

		String(const char* rhs);

		char* c_str();
		const char* c_str() const;

		void copy_from_cstr(const char* str, size_t len=size_t(-1), size_t idx=0);

		size_t length() const;

		bool operator == (const char* rhs) const;
		bool operator == (const String& rhs) const;
		String operator+(const String& rhs);
		String& operator+=(const String& rhs);
		String& operator=(const String& rhs);

	};

	/// @}

}


