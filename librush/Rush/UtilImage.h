#pragma once

#include "Rush.h"

namespace Rush
{
	/// @addtogroup util
	/// @{

	class DataStream;
	struct ColourRGBA8;

	struct PixelFormat
	{
		enum type 
		{
			R8G8B8,
			A8R8G8B8,
			B8G8R8A8,
			A16B16G16R16,
			A16B16G16R16F,
			A32B32G32R32F,
			G16R16F,
			G32R32F,
			R16F,
			R32F,
			D24S8,

			L8,
			L16,

			DXT1,
			DXT5,

			Unknown,
			_num_elements
		};

		static uint32 bits_per_pixel(PixelFormat::type fmt);
		static const char* to_string(PixelFormat::type fmt);
	};	

	class Image
	{
	public:

		Image();
		~Image();
        
        bool create(uint32 w, uint32 h, PixelFormat::type format);

		INLINE uint32 width() const { return m_width; }
		INLINE uint32 height() const { return m_height; }
		INLINE PixelFormat::type format() const { return m_format; }
		
        INLINE const uint8* data() const { return m_data; }
		INLINE uint8* data() { return m_data; }
        
        inline uint32 data_size_in_bytes() const { return m_data_size_in_bytes; }
        
		bool read_dds(DataStream& stream);
		bool load_dds(const char* filename);
        
        bool save_tga(const char* filename);

	private:

		PixelFormat::type 	m_format;

		uint32				m_width;
		uint32				m_height;

		uint8*				m_data;
        uint32              m_data_size_in_bytes;

	};

	/// @}

}


