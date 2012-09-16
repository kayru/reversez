#include "UtilIrradianceVolume.h"

#include "UtilDataStream.h"
#include "MathCommon.h"
#include "MathConstants.h"

#include <math.h>

namespace
{
	INLINE int floor_to_int(float f)
	{
		return int(floorf(f));
	}
}

namespace Rush
{
	IrradianceVolume::IrradianceVolume( const Vector3& position, const Vector3& dimensions, uint32 width, uint32 height, uint32 depth ) 
		: m_position(position)
		, m_dimensions(dimensions)
		, m_cell_size(dimensions/Vector3(float(width),float(height),float(depth)))
		, m_width(width)
		, m_height(height)
		, m_depth(depth)
		, m_cells(NULL)
	{
		m_cells = new SphericalHarmonics3[m_width*m_height*m_depth];
	}

	IrradianceVolume::~IrradianceVolume()
	{
		delete m_cells;
	}

	SphericalHarmonics3 IrradianceVolume::sample( const Vector3& pos ) const
	{
		Vector3 rel_pos = pos;

		rel_pos -= m_position;
		rel_pos += m_dimensions*0.5f;
		rel_pos -= m_cell_size*0.5f;
		rel_pos /= m_cell_size;

		int32 x0 = floor_to_int(rel_pos.x);
		int32 y0 = floor_to_int(rel_pos.y);
		int32 z0 = floor_to_int(rel_pos.z);
		int32 x1 = x0+1;
		int32 y1 = y0+1;
		int32 z1 = z0+1;
		
		float ax = rel_pos.x - float(x0);
		float ay = rel_pos.y - float(y0);
		float az = rel_pos.z - float(z0);

		if(x0<0) {x0=0; x1=x0; ax=0;}
		if(y0<0) {y0=0; y1=y0; ay=0;}
		if(z0<0) {z0=0; z1=z0; az=0;}
		if(x1>=int32(m_width))  {x1=int32(m_width)-1;  x0=x1; ax=1;}
		if(y1>=int32(m_height)) {y1=int32(m_height)-1; y0=y1; ay=1;}
		if(z1>=int32(m_depth))  {z1=int32(m_depth)-1;  z0=z1; az=1;}
				
		const SphericalHarmonics3& sh000 = cell(x0,y0,z0);
		const SphericalHarmonics3& sh100 = cell(x1,y0,z0);
		const SphericalHarmonics3& sh010 = cell(x0,y1,z0);
		const SphericalHarmonics3& sh110 = cell(x1,y1,z0);

		const SphericalHarmonics3& sh001 = cell(x0,y0,z1);
		const SphericalHarmonics3& sh101 = cell(x1,y0,z1);
		const SphericalHarmonics3& sh011 = cell(x0,y1,z1);
		const SphericalHarmonics3& sh111 = cell(x1,y1,z1);

		SphericalHarmonics3 top0 = sh000; top0.lerp(sh100, ax);
		SphericalHarmonics3 top1 = sh010; top1.lerp(sh110, ax);
		SphericalHarmonics3 top  = top0; top.lerp(top1, ay);

		SphericalHarmonics3 bot0 = sh001; bot0.lerp(sh101, ax);
		SphericalHarmonics3 bot1 = sh011; bot1.lerp(sh111, ax);
		SphericalHarmonics3 bot  = bot0; bot.lerp(bot1, ay);

		SphericalHarmonics3 res = top; res.lerp(bot, az);
		
		return res;
	}

	Vector3 IrradianceVolume::cell_position( uint32 x, uint32 y, uint32 z ) const
	{		
		Vector3 res = Vector3(float(x),float(y),float(z));

		res *= m_cell_size;
		res += m_position;
		res += m_cell_size*0.5f;
		res -= m_dimensions*0.5f;

		return res;
	}

	Vector3 IrradianceVolume::cell_position( uint32 idx ) const
	{
		uint32 z = idx / (m_width*m_height);
		uint32 y = (idx-z*(m_width*m_height)) / m_width;
		uint32 x = (idx-z*(m_width*m_height) - y*m_width);

		return cell_position(x,y,z);	
	}

	bool IrradianceVolume::read( DataStream& stream )
	{
		delete m_cells;
		
		stream.read_t(m_width);
		stream.read_t(m_height);
		stream.read_t(m_depth);

		stream.read_t(m_position);
		stream.read_t(m_dimensions);
		stream.read_t(m_cell_size);
		
		const uint32 size = num_cells();

		m_cells = new SphericalHarmonics3[size];

		for(uint32 i=0; i<size; ++i )
		{
			stream.read_t(m_cells[i]);
		}

		return true;
	}

	void IrradianceVolume::write( DataStream& stream )
	{
		stream.write_t(m_width);
		stream.write_t(m_height);
		stream.write_t(m_depth);

		stream.write_t(m_position);
		stream.write_t(m_dimensions);
		stream.write_t(m_cell_size);

		uint32 size = num_cells();
		for(uint32 i=0; i<size; ++i )
		{
			stream.write_t(m_cells[i]);
		}
	}

	void IrradianceVolume::add_point_light( const Vector3& pos, float radius, const Vector3& colour )
	{
		Vector3 cell_origin = cell_position(0,0,0);
		Vector3 cell_pos = cell_origin;
		Vector3 cell_step = cell_position(1,1,1) - cell_pos;
		uint32 cell_idx = 0;

		for( uint32 z = 0; z<depth(); ++z )
		{
			cell_pos.y = cell_origin.y;
			for( uint32 y = 0; y<height(); ++y )
			{
				cell_pos.x = cell_origin.x;
				for( uint32 x = 0; x<width(); ++x )
				{
					SphericalHarmonics3& sh = cell(cell_idx);

					Vector3 to_light  = cell_pos - pos;
					float dist	  = to_light.length();
					Vector3 light_dir = to_light / dist;

					float falloff   = float(1.0f-rush_saturate(dist / radius));

					Vector3 attenuated_colour = colour * falloff * falloff * PI;

					if(falloff>0.0f)
					{
						sh.add(light_dir, attenuated_colour);
					}

					cell_pos.x += cell_step.x;
					++cell_idx;
				}
				cell_pos.y += cell_step.y;
			}
			cell_pos.z += cell_step.z;
		}
	}

}
// EOF
