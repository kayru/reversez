#include "UtilStdModel.h"
#include "UtilDataStream.h"
#include "UtilFNV1.h"

namespace Rush
{
	template <typename LIST_TYPE>
	static bool stdmodel_write_list(const LIST_TYPE& arr, DataStream& stream)
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
	static bool stdmodel_read_list(LIST_TYPE& arr, DataStream& stream)
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

	bool StdModel::read( DataStream& stream )
	{
		uint32 stream_version;
		stream.read_t(stream_version);

		if( stream_version == version )
		{
			stdmodel_read_list(materials, stream);
			stdmodel_read_list(vertices, stream);
			stdmodel_read_list(indices, stream);
			stdmodel_read_list(submeshes, stream);

			for( uint32 i=0; i<materials.size(); ++i )
			{
				if( materials[i].material_hash_fnv1 == 0 )
				{
					materials[i].update_hash();
				}
			}

			return true;
		}
		else 
		{
			return false;
		}
	}

	void StdModel::write( DataStream& stream )
	{
		uint32 v = version;
		stream.write(&v, sizeof(v));

		stdmodel_write_list(materials, stream);
		stdmodel_write_list(vertices, stream);
		stdmodel_write_list(indices, stream);
		stdmodel_write_list(submeshes, stream);
	}

	void StdMaterial::update_hash()
	{
		Hasher_FNV1 hasher;
		hasher.hash((char*)this+sizeof(material_hash_fnv1), sizeof(*this)-sizeof(material_hash_fnv1));
		material_hash_fnv1 = hasher.result();
	}
}
