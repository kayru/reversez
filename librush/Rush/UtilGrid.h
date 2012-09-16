#pragma once

#include "Rush.h"
#include "UtilAssert.h"

namespace Rush
{

	/// @addtogroup util
	/// @{

	template <typename T>
	class Grid
	{
	public:

		Grid();
		Grid(uint32 width, uint32 height);
		~Grid();
		
		INLINE uint32 width()	const { return m_width; }
		INLINE uint32 height()	const { return m_height; }

		INLINE uint32 num_cells() const { return m_width*m_height; }

		INLINE void	resize(uint32 width, uint32 height);

		INLINE T& operator () (uint32 x, uint32 y)
		{
			R_ASSERT(x<m_width);
			R_ASSERT(y<m_height);

			return m_data[x+y*m_width];
		}

		INLINE const T& operator () (uint32 x, uint32 y) const 
		{
			R_ASSERT(x<m_width);
			R_ASSERT(y<m_height);

			return m_data[x+y*m_width];
		}

		void fill(const T& val);

	private:

		uint32	m_width;
		uint32	m_height;
		
		T*		m_data;

	};

	//////////////////////////////////////////////////////////////////////////

	template <typename T>
	void Rush::Grid<T>::fill( const T& val )
	{
		for( uint32 y=0; y<m_height; ++y )
		{
			for( uint32 x=0; x<m_width; ++x )
			{
				m_data[x+y*m_width] = val;
			}
		}
	}

	
	template <typename T>
	Rush::Grid<T>::Grid() :
		m_width(0),
		m_height(0),
		m_data(0)
	{

	}
	template <typename T>
	Rush::Grid<T>::Grid( uint32 width, uint32 height ):
		m_width(0),
		m_height(0),
		m_data(0)
	{
		resize(width, height);
	}

	template <typename T>
	Rush::Grid<T>::~Grid()
	{
		delete[] m_data;
	}

	template <typename T>
	void Rush::Grid<T>::resize( uint32 width, uint32 height )
	{
		delete m_data;

		m_width  = width;
		m_height = height;

		m_data = new T[m_width*m_height];
	}

	/// @}

}

