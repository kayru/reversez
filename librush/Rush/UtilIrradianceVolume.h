#pragma once

#include "Rush.h"
#include "MathSphericalHarmonics.h"

namespace Rush
{
	/// @addtogroup util
	/// @{

	class DataStream;
	class IrradianceVolume
	{
	public:

		IrradianceVolume(const Vector3& position, const Vector3& dimensions, uint32 width, uint32 height, uint32 depth);
		~IrradianceVolume();

		INLINE const Vector3& position() const { return m_position; }
		INLINE const Vector3& dimensions() const { return m_dimensions; }
		INLINE const Vector3& cell_size() const { return m_cell_size; }

		INLINE uint32 width() const { return m_width; }
		INLINE uint32 height() const { return m_height; }
		INLINE uint32 depth() const { return m_depth; }

		INLINE uint32 cell_index(uint32 x, uint32 y, uint32 z) const { return x + y*m_width + z*m_width*m_height; }

		INLINE SphericalHarmonics3& cell(uint32 x, uint32 y, uint32 z) { return m_cells[cell_index(x,y,z)]; }
		INLINE SphericalHarmonics3& cell(uint32 idx) { return m_cells[idx]; }

		INLINE const SphericalHarmonics3& cell(uint32 x, uint32 y, uint32 z) const { return m_cells[cell_index(x,y,z)]; }
		INLINE const SphericalHarmonics3& cell(uint32 idx) const { return m_cells[idx]; }

		INLINE uint32 num_cells() const { return m_width * m_height * m_depth; }
						
		SphericalHarmonics3	sample(const Vector3& pos) const;						
		Vector3				cell_position(uint32 x, uint32 y, uint32 z) const;
		Vector3				cell_position(uint32 idx) const;

		void add_point_light(const Vector3& pos, float radius, const Vector3& colour);

		bool read(DataStream& stream);
		void write(DataStream& stream);

	private:

		RUSH_DISALLOW_COPY_AND_ASSIGN(IrradianceVolume);

		Vector3 m_position;
		Vector3 m_dimensions;
		Vector3 m_cell_size;
		uint32  m_width;
		uint32  m_height;
		uint32  m_depth;

		SphericalHarmonics3* m_cells;
	};

	/// @}

}

// EOF
