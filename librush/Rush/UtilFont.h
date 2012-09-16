#pragma once

#include "Rush.h"

namespace Rush
{
	/// @addtogroup util
	/// @{

	class DataStream;
	
	// Class for working with AngelCode BMFont files.
	// More details here: http://www.angelcode.com/products/bmfont/
	// Current simple implementation is only suitable for ASCI single-byte encoding.
	class Font
	{	
	public:

		enum { MaxFontPages=16, InvalidPage=0xFF };

		struct CharData
		{			
			uint16	x, y, width, height;
			int16	offset_x, offset_y, advance_x;
			uint8	page, chan;			
		};

		struct PageData
		{
			char filename[128];
		};

		Font();

		bool read(DataStream& stream);

		INLINE const CharData&	get_char(uint8 c) const { return m_chars[c]; }					
		INLINE const char*		get_page(uint32 n) const { return m_pages[n].filename; }
		INLINE uint32			num_pages() const { return m_num_pages; }
		
		INLINE uint32			size() const { return m_size; }

	private:

		RUSH_DISALLOW_COPY_AND_ASSIGN(Font);
		
		CharData	m_chars[256];
		PageData	m_pages[MaxFontPages];
		uint32		m_num_pages;
		uint32		m_size;
	};

	/// @}

}


