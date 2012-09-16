#pragma once

#include "UtilAssert.h"

namespace Rush
{

	/// @addtogroup util
	/// @{

	// This class should be used purely for debugging and error detection purposes.
	// It is not supposed to be a classic reference counted pointer scheme.
	class ReferenceCount
	{

	public:

		ReferenceCount() : m_count(0)
		{
		}

		~ReferenceCount()
		{
			R_ASSERT(m_count==0);
		}

		uint32 get_reference_count() const 
		{
			return m_count;
		}

		void add_reference()
		{
			m_count++;
		}

		void remove_reference()
		{
			R_ASSERT(m_count!=0);
			m_count--;
		}
		
	private:

		uint32 m_count;

	};

	/// @}
}


