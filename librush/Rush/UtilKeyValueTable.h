#pragma once

#include "UtilDynamicArray.h"

namespace Rush
{	
	/// @addtogroup util
	/// @{

	// simple key-value table, with linear search
	template<typename KEY_TYPE, typename VALUE_TYPE>
	class KeyValueTable
	{

	public:
		
		INLINE KeyValueTable()
		{
		}
		
		INLINE ~KeyValueTable()
		{
			clear();
		}

		INLINE VALUE_TYPE* set_guaranteed_not_existing(const KEY_TYPE& key, const VALUE_TYPE& val)
		{
			keys.push_back(key);
			values.push_back(val);
			return &values.back();
		}

		INLINE VALUE_TYPE* set(const KEY_TYPE& key, const VALUE_TYPE& val)
		{
			VALUE_TYPE* existing = get(key);
			if( existing )
			{
				*existing = val;
				return existing;
			}
			else
			{				
				return set_guaranteed_not_existing(key, val);
			}
		}

		INLINE VALUE_TYPE* get(const KEY_TYPE& key)
		{
			for( uint32 i=0; i<keys.size(); ++i )
			{				
				const KEY_TYPE& curr_key = keys[i];
				if( !(curr_key == key) ) continue;
				return &values[i];
			}

			return NULL;
		}

		INLINE const VALUE_TYPE* get(const KEY_TYPE& key) const 
		{
			for( uint32 i=0; i<keys.size(); ++i )
			{		
				const KEY_TYPE& curr_key = keys[i];
				if( !(curr_key == key) ) continue;
				return &values[i];
			}
			return NULL;
		}

		INLINE bool contains(const KEY_TYPE& key) const
		{
			return get(key)!=NULL;
		}

		INLINE void clear()
		{
			keys.clear();
			values.clear();
		}

		INLINE uint32 size() const 
		{
			return values.size();
		}

		INLINE const VALUE_TYPE& get_value_by_index(uint32 idx)
		{
			return values[idx];
		}

		INLINE const VALUE_TYPE& get_key_by_index(uint32 idx)
		{
			return keys[idx];
		}

		INLINE VALUE_TYPE& operator[] (uint32 i)
		{
			return values[i];
		}

		INLINE const VALUE_TYPE& operator[](uint32 i) const
		{
			return values[i];
		}

	private:

		DynamicArray<KEY_TYPE>		keys;
		DynamicArray<VALUE_TYPE>	values;

	};

	/// @}

}


// EOF
