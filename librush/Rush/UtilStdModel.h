#pragma once

//todo: rename this to OfflineModel

#include "Rush.h"
#include "MathVector3.h"
#include "UtilDynamicArray.h"

namespace Rush
{
	/// @addtogroup util
	/// @{

	class DataStream;

	struct StdVertex
	{
		Vector3 pos;
		Vector2 tex;
		Vector3 nor;
		Vector3 bin;
		Vector3 tan;
	};

	struct StdMaterial
	{
		enum { MaxStringLength=256 };
		
		uint32 material_hash_fnv1;

		char id[MaxStringLength];

		char map_diffuse[MaxStringLength];
		char map_specular[MaxStringLength];
		char map_bump[MaxStringLength];		

		Vector3 col_diffuse;

		inline StdMaterial()
		{
			material_hash_fnv1 = 0;
			id[0]=0;
			map_diffuse[0]=0;
			map_specular[0]=0;
			map_bump[0]=0;
		}

		void update_hash();
	};

	struct StdSubmesh
	{	
		enum { MaxStringLength=256 };

		char id[MaxStringLength];
		char material[MaxStringLength];

		uint32 idx_mat;
		uint32 idx_first;
		uint32 idx_last;
	};

	struct StdModel
	{
		enum { version = 2 };

		DynamicArray<StdMaterial>	materials;
		DynamicArray<StdVertex>		vertices;
		DynamicArray<uint32>		indices;
		DynamicArray<StdSubmesh>	submeshes;
		
		bool read(DataStream& stream);
		void write(DataStream& stream);
	};

	/// @}

}


