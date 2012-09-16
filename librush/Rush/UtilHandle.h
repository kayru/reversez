#pragma once

#include "Rush.h"

namespace Rush
{
	template <typename T>
	class Handle
	{

	public:

		INLINE Handle(int32 index=-1) : m_index(index) {}
		INLINE int32 index() const { return m_index; }

		INLINE bool valid() const { return m_index!=-1; }
		INLINE void invalidate() { m_index = -1; }

		INLINE bool operator == (const Handle<T>& rhs) const { return m_index == rhs.m_index; }
		INLINE bool operator != (const Handle<T>& rhs) const { return m_index != rhs.m_index; }

	private:

		int32 m_index;

	};
}
