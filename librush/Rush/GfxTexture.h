#pragma once

#include "UtilImage.h"

namespace Rush
{
	struct TextureType
	{
		enum type 
		{
			Tex2D,
			Tex3D,
			TexCUBE
		};
	};

	struct TextureDescr
	{	
		inline TextureDescr()
			: width(0)
			, height(0)
			, depth(0)
			, format(PixelFormat::Unknown)
			, type(TextureType::Tex2D)
			, render_target(false)
			, dynamic(false)
		{

		}

		static inline TextureDescr create_2D(uint32 _width, uint32 _height, PixelFormat::type _format = PixelFormat::A8R8G8B8, bool _render_target=false, bool _dynamic=false)
		{
			TextureDescr res;
			res.width = _width;
			res.height = _height;
			res.depth = 0;
			res.format = _format;
			res.type = TextureType::Tex2D;
			res.render_target = _render_target;
			res.dynamic = _dynamic;
			return res;
		}
		static inline TextureDescr create_3D(uint32 _width, uint32 _height, uint32 _depth, PixelFormat::type _format = PixelFormat::A8R8G8B8, bool _render_target=false, bool _dynamic=false)
		{
			TextureDescr res;
			res.width = _width;
			res.height = _height;
			res.depth = _depth;
			res.format = _format;
			res.type = TextureType::Tex3D;
			res.render_target = _render_target;
			res.dynamic = _dynamic;
			return res;
		}
		static inline TextureDescr create_CUBE(uint32 _size, PixelFormat::type _format = PixelFormat::A8R8G8B8, bool _dynamic=false)
		{
			TextureDescr res;
			res.width = _size;
			res.height = _size;
			res.depth = 0;
			res.format = _format;
			res.type = TextureType::TexCUBE;
			res.render_target = false;
			res.dynamic = _dynamic;
			return res;
		}

		inline bool operator == (const TextureDescr& rhs)
		{ 
			return	width == rhs.width &&
					height == rhs.height &&
					depth == rhs.depth &&
					format == rhs.format &&
					type == rhs.type &&
					render_target == rhs.render_target &&
					dynamic == rhs.dynamic;
		}

		inline bool operator != (const TextureDescr& rhs) { return !(*this == rhs); }
		
		uint32				width;
		uint32				height;
		uint32				depth;
		PixelFormat::type	format;
		TextureType::type	type;
		bool				render_target;
		bool				dynamic;
	};

}

