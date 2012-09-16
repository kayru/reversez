#pragma once

#include "UtilGrid.h"

namespace Rush
{
	/// @addtogroup util
	/// @{

	template <typename T>
	class SparseGrid
	{

		typedef Grid<T> Cell;

	public:

		SparseGrid(uint32 cell_width, uint32 cell_height, uint32 num_cells_x, uint32 num_cells_y, const T& initial_val);
		~SparseGrid();

		INLINE uint32 width()  const { return m_width;  }
		INLINE uint32 height() const { return m_height; }

		INLINE const T&	get(uint32 x, uint32 y) const;
		INLINE void		set(uint32 x, uint32 y, const T& val);


	private:

		Grid<Cell>	m_cells;

		uint32		m_width;
		uint32		m_height;

		uint32		m_cell_width;
		uint32		m_cell_height;

		uint32		m_num_cells_x;
		uint32		m_num_cells_y;

	};

	template <typename T>
	void Rush::SparseGrid<T>::set( uint32 x, uint32 y, const T& val )
	{
		uint32 cell_x = x/m_cell_width;
		uint32 cell_y = y/m_cell_height;

		Cell& c = m_cells(cell_x, cell_y);

		uint32 cx = x%m_cell_width;
		uint32 cy = y%m_cell_height;

		if( c.num_cells() == 1 )
		{			
			if( c(0,0)!=val )
			{
				T old_val = c(0,0);
				c.resize(m_cell_width, m_cell_height);
				c.fill(old_val);
				c(cx,cy) = val;
			}
		}
		else
		{			
			c(cx,cy) = val;
		}
	}

	template <typename T>
	const T& Rush::SparseGrid<T>::get( uint32 x, uint32 y ) const
	{
		uint32 cell_x = x/m_cell_width;
		uint32 cell_y = y/m_cell_height;

		const Cell& c = m_cells(cell_x, cell_y);

		if( c.num_cells() == 1 )
		{			
			return c(0,0);
		}
		else
		{			
			uint32 cx = x%m_cell_width;
			uint32 cy = y%m_cell_height;

			return c(cx,cy);
		}
	}

	template <typename T>
	Rush::SparseGrid<T>::~SparseGrid()
	{	

	}

	template <typename T>
	Rush::SparseGrid<T>::SparseGrid( uint32 cell_width, uint32 cell_height, uint32 num_cells_x, uint32 num_cells_y, const T& initial_val ) :
	m_width(cell_width*num_cells_x),
		m_height(cell_height*num_cells_y),
		m_cell_width(cell_width),
		m_cell_height(cell_height),
		m_num_cells_x(num_cells_x),
		m_num_cells_y(num_cells_y)
	{
		m_cells.resize(num_cells_x, num_cells_y);
		for( uint32 j=0; j<num_cells_y; ++j )
		{
			for( uint32 i=0; i<num_cells_x; ++i )
			{
				m_cells(i,j).resize(1,1);
				m_cells(i,j)(0,0) = initial_val;
			}
		}
	}

	/// @}
}


