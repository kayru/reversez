#pragma once

namespace Rush
{	
	struct BufferMode
	{
		enum type 
		{
			WriteOnce,
			WriteRarely,
			WriteOften,

			_num_elements
		};
	};

	struct IndexFormat
	{
		enum type 
		{
            Unknown,
			Index16,
			Index32
		};

		static inline uint32 get_stride(type t)
		{
			switch(t)
			{
				case Index16: return sizeof(uint16);
				case Index32: return sizeof(uint32);
				default: return 0;
			}
		};
	};

	struct IndexBufferDescr
	{
		inline IndexBufferDescr(){}
		inline IndexBufferDescr(BufferMode::type _mode, uint32 _count, IndexFormat::type _type)
			: mode(_mode)
			, count(_count)
			, type(_type)
			, stride(IndexFormat::get_stride(_type))
			, size(_count*IndexFormat::get_stride(_type))
		{}

		BufferMode::type mode;
		uint32 count;
		IndexFormat::type type;
		uint32 stride;
		uint32 size;
	};

	struct VertexBufferDescr
	{
		inline VertexBufferDescr(){}
		inline VertexBufferDescr(BufferMode::type _mode, uint32 _count, uint32 _stride)
			: mode(_mode)
			, count(_count)
			, stride(_stride)
			, size(_count*_stride)
		{}

		BufferMode::type mode;
		uint32 count;
		uint32 stride;
		uint32 size;
	};	
}
