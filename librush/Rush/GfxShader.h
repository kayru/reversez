#pragma once

#include "Rush.h"
#include "UtilStaticArray.h"
#include "UtilDynamicArray.h"
#include "UtilMemory.h"
#include "UtilString.h"
#include "GfxVertexFormat.h"
#include "GfxCommon.h"

namespace Rush
{	
	class RenderDevice;
	class ShaderDescr;	
	
	struct Matrix44;
	struct Vector4;

	struct ShaderType
	{
		enum type 
		{
			Vertex,
			Pixel,
			Geometry,
			Hull,
			Domain,
			Compute
		};
	};

	struct ShaderConstantIndex
	{
		inline ShaderConstantIndex()
			: index(uint32(-1))
			, count(uint32(-1))
		{}

		inline ShaderConstantIndex(uint32 _index, uint32 _count=1)
			: index(_index)
			, count(_count)
		{}

		bool is_valid() const { return index!=uint32(-1); }

		uint32 index;
		uint32 count;
	};

	struct ShaderConstantDescr
	{		
		String 				name;
		uint32 				register_index;
		uint32 				register_count;

		ShaderConstantDescr()
			: name()
			, register_index(0)
			, register_count(0)
		{
		}

		inline bool operator == (const ShaderConstantDescr& rhs) const
		{
			return	register_index == rhs.register_index &&
					register_count == rhs.register_count &&
					name == rhs.name;
		}
	};

	struct ShaderConstantTable : public DynamicArray<ShaderConstantDescr>
	{
		ShaderConstantIndex index_from_name(const char* name) const
		{
			ShaderConstantIndex res;
			for( uint32 i=0; i<size(); ++i )
			{
				if( element(i).name == name )
				{
					res.index = element(i).register_index;
					res.count = element(i).register_count;
				}
			}
			return res;
		}
	};

	enum ShaderCodeType
	{
		ShaderCodeType_Binary,
		ShaderCodeType_Text
	};

	struct ShaderCode : public DynamicArray<char>
	{
		ShaderCode(): type(ShaderCodeType_Binary){}

		ShaderCode(ShaderCodeType _type, const char* _code, size_t _size) : type(_type)
		{
			if( _type == ShaderCodeType_Text && _size==0 )
			{
				_size = (size_t)rush_strlen(_code)+1;
				this->resize(_size);
				rush_memcpy(this->data(), _code, _size);
			}
			else
			{
				this->resize(_size);
				rush_memcpy(this->data(), _code, _size);
			}
		}

		ShaderCodeType type;
	};

	struct ShaderCodeText : public ShaderCode
	{
		ShaderCodeText(const char* _text, size_t _length=0)
			: ShaderCode(ShaderCodeType_Text, _text, _length)
		{}
	};

	struct ShaderCodeBinary : public ShaderCode
	{
		ShaderCodeBinary(const char* _bytes, size_t _size)
			: ShaderCode(ShaderCodeType_Binary, _bytes, _size)
		{}
	};

	struct ShaderConstantBindings
	{
		enum BindingType
		{
			BindingType_Constant,
			BindingType_Sampler
		};

		struct Item
		{
			const char* name;
			const Vector4* data;
			uint32 count;
			uint32 idx;
			BindingType type;
		};

		bool add_constant(const char* name, const Vector4* data, uint32 count)
		{
			Item it;
			it.name = name;
			it.data = data;
			it.count = count;
			it.idx = 0;
			it.type = BindingType_Constant;
			return items.push_back(it);
		}

		bool add_sampler(const char* name, uint32 idx)
		{
			Item it;
			it.name = name;
			it.data = NULL;
			it.count = 1;
			it.idx = idx;
			it.type = BindingType_Sampler;
			return items.push_back(it);
		}

		StaticArray<Item, 128> items;
	};

	struct TechniqueDescr
	{
		TechniqueDescr(PixelShaderHandle a_ps=InvalidResourceHandle(), VertexShaderHandle a_vs=InvalidResourceHandle(), 
                        VertexFormatHandle a_vf=InvalidResourceHandle(), const ShaderConstantBindings* a_bindings=NULL)
			: ps(a_ps)
            , vs(a_vs)
            , vf(a_vf)
            , bindings(a_bindings)
		{}

		PixelShaderHandle				ps;
		VertexShaderHandle				vs;
		VertexFormatHandle				vf;
		const ShaderConstantBindings*	bindings;
	};

}
