#pragma once

#include "Rush.h"
#include "MathVector3.h"
#include "UtilDynamicArray.h"
#include "UtilMemory.h"
#include "UtilHash.h"

namespace Rush
{
	/// @addtogroup graphics
	/// @{

	class DataStream;

	struct OfflineVertex
	{
		Vector3 pos;
		Vector2 tex;
		Vector3 nor;
		Vector3 bin;
		Vector3 tan;
		uint32	col;
		Vector2 tex2;
	};

	struct OfflineMaterial
	{
		enum { MaxStringLength=256 };
		
		MD5Hash material_hash_md5;

		char id[MaxStringLength];

		char map_diffuse[MaxStringLength];
		char map_specular[MaxStringLength];
		char map_gloss[MaxStringLength];
		char map_normal[MaxStringLength];
		char map_emissive[MaxStringLength];
		char map_occlusion[MaxStringLength];

		Vector3 col_diffuse;

		float specular_level;
		float specular_power;
		
		float emissive_level;

		inline OfflineMaterial() { set_defaults(); }

		void update_hash();

		void set_defaults();
	};

	struct OfflineModelSegment
	{
		enum { MaxStringLength=256 };

		char id[MaxStringLength];
		char material[MaxStringLength];

		uint32 idx_mat;
		uint32 idx_first;
		uint32 idx_last;
	};

	struct OfflineModel
	{
		enum { VERSION = 8 };

		DynamicArray<OfflineMaterial>		materials;
		DynamicArray<OfflineVertex>			vertices;
		DynamicArray<uint32>				indices;
		DynamicArray<OfflineModelSegment>	segments;

		Vector3								bbox_min;
		Vector3								bbox_max;

		bool read(DataStream& stream);
		void write(DataStream& stream) const;

		void calculate_bbox();
		void calculate_tbn();

	};

	/// @}
}
