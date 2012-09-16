#include "UtilString.h"

namespace Rush
{
	String::String() : DynamicArray<char>()
	{
	}

	String::String( const String& rhs )
	{
		copy_from_cstr(rhs.c_str(), rhs.length());
	}

	String::String( const char* rhs )
	{
		copy_from_cstr(rhs);
	}

	String::~String()
	{

	}

	char* String::c_str()
	{
		if( size() ) return data();
		else return NULL;
	}

	const char* String::c_str() const
	{
		if( size() ) return data();
		else return NULL;
	}

    void String::copy_from_cstr( const char* str, size_t len/*=size_t(-1)*/, size_t idx/*=0*/ )
	{
		if( str )
		{
			if( len == size_t (-1) )
			{
				len = (size_t)rush_strlen(str);
			}
			size_t new_len = idx + len;
			resize(new_len+1);
			char* dst = &(*this)[idx];
			rush_memcpy(dst, str, len+1);
		}
		else
		{
			resize(1);
			(*this)[0] = 0;
		}
	}

	size_t String::length() const
	{
		size_t len = size();
		if( len == 0 ) return 0;
		else return len-1;
	}


	bool String::operator == (const char* rhs) const
	{
		if( this->length() )
		{
			return rush_strcmp(data(), rhs) == 0;
		}
		else
		{
			return false;
		}
	}

	bool String::operator == (const String& rhs) const
	{
		if( this->length() != rhs.length() )
		{
			return false;
		}

		if( this->length() )
		{
			return rush_strcmp(data(), rhs.data()) == 0;
		}
		else
		{
			return false;
		}
	}

	String String::operator+(const String& rhs)
	{
		String res;

		res.copy_from_cstr(data(), length());
		res.copy_from_cstr(rhs.data(), rhs.length(), res.length());

		return res;
	}

	String& String::operator+=(const String& rhs)
	{
		this->copy_from_cstr(rhs.data(), rhs.length(), length());
		return *this;
	}

	String& String::operator=(const String& rhs)
	{
		copy_from_cstr(rhs.c_str(), rhs.length());
		return *this;
	}


}