#pragma once

#include "Rush.h"
#include "UtilMemory.h"
#include "UtilStaticArray.h"
#include "UtilAssert.h"

namespace Rush
{
    struct InvalidResourceHandle {};

    struct UntypedResourceHandle
    {
        UntypedResourceHandle(size_t _index) : index(_index) {}
        size_t index;
    };

    template <typename T>
    class ResourceHandle
    {
    public:

        ResourceHandle() : m_index(0) {}
        ResourceHandle(InvalidResourceHandle) : m_index(0) {}
        explicit ResourceHandle(UntypedResourceHandle h) : m_index(h.index) {}

        bool valid() const { return m_index!=0; }
        size_t index() const { return m_index; }

        bool operator == (const ResourceHandle<T>& rhs) const { return m_index == rhs.m_index; }
        bool operator != (const ResourceHandle<T>& rhs) const { return m_index != rhs.m_index; }

    private:

        ResourceHandle(size_t idx) : m_index(idx) {}

        size_t m_index;

    };

    template <typename T, typename HANDLE_TYPE, size_t CAPACITY>
    class ResourcePool
    {
    public:

        typedef HANDLE_TYPE HandleType;

        ResourcePool()
        {
            T default_value;
            rush_memset(&default_value, 0, sizeof(default_value));

            for(size_t i=CAPACITY; i!=0; --i)
            {
                empty.push_back(i-1);
            }

            push(default_value);
        }

        INLINE HANDLE_TYPE push(const T& val)
        {
            R_ASSERT(empty.size()!=0);
            size_t idx = empty.back();
            empty.pop_back();
            data[idx] = val;
            return HANDLE_TYPE(UntypedResourceHandle(idx));
        }

        INLINE void remove(HANDLE_TYPE h)
        {
            if( h.valid() ) 
            {
                empty.push_back(h.index());
            }
        }

        INLINE const T& operator [] (HANDLE_TYPE h) const
        {
            return data[h.index()];
        }

        INLINE T& operator [] (HANDLE_TYPE h)
        {
            return data[h.index()];
        }

        INLINE uint32 capacity() const { return CAPACITY; }

    private:

        T data[CAPACITY];
        StaticArray<size_t, CAPACITY> empty;

    };


}

