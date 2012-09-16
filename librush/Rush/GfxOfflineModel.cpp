#include "GfxOfflineModel.h"
#include "UtilDataStream.h"
#include "UtilMD5.h"
#include "UtilLog.h"
#include "MathAABB3.h"
#include "MathConstants.h"

namespace Rush
{
	template <typename LIST_TYPE>
	static bool write_list(const LIST_TYPE& arr, DataStream& stream)
	{
		uint32 count = (uint32)arr.size();
		stream.write(&count, sizeof(count));
		if( count )
		{
			stream.write(&arr[0], (uint32)(sizeof(arr[0]) * arr.size()));
		}
		return true;
	}

	template <typename LIST_TYPE>
	static bool read_list(LIST_TYPE& arr, DataStream& stream)
	{
		uint32 count;
		stream.read(&count, sizeof(count));
		if( count )
		{
			arr.resize(count);
			stream.read(&arr[0], (uint32)(sizeof(arr[0]) * arr.size()));
		}
		return true;
	}

	void OfflineMaterial::update_hash()
	{
		Hasher_MD5 hasher((char*)this+sizeof(material_hash_md5), sizeof(*this)-sizeof(material_hash_md5));
		material_hash_md5 = hasher.get();
	}

	void OfflineMaterial::set_defaults()
	{
		material_hash_md5.data[0] = 0;
		material_hash_md5.data[1] = 0;
		material_hash_md5.data[2] = 0;
		material_hash_md5.data[3] = 0;

		id[0] = 0;
		map_diffuse[0] = 0;
		map_specular[0] = 0;
		map_gloss[0] = 0;
		map_normal[0] = 0;
		map_emissive[0] = 0;
		map_occlusion[0] = 0;

		col_diffuse = Vector3(1,1,1);

		specular_level = 1.0f;
		specular_power = 50.0f;

		emissive_level = 0.0f;
	}

	bool OfflineModel::read( DataStream& stream )
	{
		uint32 stream_version;
		stream.read(&stream_version, sizeof(stream_version));

		if( stream_version == VERSION )
		{
			read_list(materials, stream);
			read_list(vertices, stream);
			read_list(indices, stream);
			read_list(segments, stream);
			stream.read_t(bbox_min);
			stream.read_t(bbox_max);

			MD5Hash invalid_hash;
			rush_memset(&invalid_hash, 0, sizeof(invalid_hash));

			for( uint32 i=0; i<materials.size(); ++i )
			{
				if( materials[i].material_hash_md5 == invalid_hash )
				{
					materials[i].update_hash();
				}
			}

			return true;
		}
		else 
		{
			Log::error("OfflineModel version miss-match");
			return false;
		}
	}

	void OfflineModel::write( DataStream& stream ) const
	{
		uint32 version = VERSION;
		stream.write(&version, sizeof(version));
		write_list(materials, stream);
		write_list(vertices, stream);
		write_list(indices, stream);
		write_list(segments, stream);
		stream.write_t(bbox_min);
		stream.write_t(bbox_max);
	}

	void OfflineModel::calculate_bbox()
	{
		AABB3 bounds(Vector3(LARGE_FLOAT, LARGE_FLOAT, LARGE_FLOAT), Vector3(-LARGE_FLOAT, -LARGE_FLOAT, -LARGE_FLOAT));
		for( uint32 i=0; i<vertices.size(); ++i )
		{
			const OfflineVertex& v = vertices[i];
			bounds.expand(v.pos);
		}
		bbox_min = bounds.vmin();
		bbox_max = bounds.vmax();
	}

	static void calculate_tangent_and_binormal(OfflineVertex& vert_a, const OfflineVertex& vert_b, const OfflineVertex& vert_c, Vector3& out_tangent, Vector3& out_binormal)
	{
		// As per: http://area.autodesk.com/blogs/chris/how_the_3ds_max_scanline_renderer_computes_tangent_and_binormal_vectors_for_normal_mapping

		float uva = vert_b.tex.x - vert_a.tex.x;
		float uvb = vert_c.tex.x - vert_a.tex.x;

		float uvc = vert_b.tex.y - vert_a.tex.y;
		float uvd = vert_c.tex.y - vert_a.tex.y;

		float uvk = uvb*uvc - uva*uvd;

		Vector3 v1 = vert_b.pos - vert_a.pos;
		Vector3 v2 = vert_c.pos - vert_a.pos;

		if( uvk!=0 ) 
		{
			out_tangent = ((v2*uvc-v1*uvd)/uvk).normalised();
		}
		else 
		{
			if (uva!=0)
			{
				out_tangent = (v1/uva).normalised();
			}
			else if (uvb!=0)
			{
				out_tangent = (v2/uvb).normalised();
			}
			else
			{
				out_tangent = Vector3(1.0f,0.0f,0.0f);
			}
		}

		Vector3 normal = (vert_b.pos - vert_a.pos).cross(vert_c.pos - vert_b.pos).normalised();

		out_binormal = out_tangent.cross(normal);
	}

	void OfflineModel::calculate_tbn()
	{
		
		// assume mesh is a triangle list

		for( uint32 idx = 0; idx < vertices.size(); ++idx )
		{
			vertices[idx].tan = 0.0f;
			vertices[idx].bin = 0.0f;
		}

		for( uint32 idx = 0; idx < indices.size(); idx+=3 )
		{
			uint32 idx_a = indices[idx + 0];
			uint32 idx_b = indices[idx + 1];
			uint32 idx_c = indices[idx + 2];

			OfflineVertex& vert_a = vertices[idx_a];
			OfflineVertex& vert_b = vertices[idx_b];
			OfflineVertex& vert_c = vertices[idx_c];

			Vector3 tangent;
			Vector3 binormal;

			calculate_tangent_and_binormal(vert_a, vert_b, vert_c, tangent, binormal);

			vert_a.bin += binormal;
			vert_a.tan += tangent;

			vert_b.bin += binormal;
			vert_b.tan += tangent;

			vert_c.bin += binormal;
			vert_c.tan += tangent;
		}

		for( uint32 idx = 0; idx < vertices.size(); ++idx )
		{
			Vector3 tangent  = vertices[idx].tan;
			Vector3 binormal = vertices[idx].bin;

			float tan_len = tangent.length();
			float bin_len = binormal.length();

			if( tan_len > 0 ) vertices[idx].tan = tangent/tan_len;
			else vertices[idx].tan = Vector3(1,0,0);

			if( bin_len > 0 ) vertices[idx].bin = binormal/bin_len;
			else vertices[idx].bin = Vector3(0,1,0);
		}
	}

}
