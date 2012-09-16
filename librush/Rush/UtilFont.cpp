#include "UtilFont.h"
#include "UtilDataStream.h"
#include "UtilLog.h"
#include "UtilEndian.h"
#include "UtilFourCC.h"
#include "UtilDynamicArray.h"
#include "MathCommon.h"

#include <stdlib.h>
#include <string.h>
#include <math.h>

namespace Rush
{

	struct BlockType
	{
		enum type 
		{
			Info	= 1,
			Common	= 2,
			Pages	= 3,
			Chars	= 4,
			Kerning	= 5
		};
	};

#pragma pack(push)
#pragma pack(1)
	
	struct InfoBlock
	{
		int16	font_size;
 		uint8	flags;
		uint8	char_set;
		uint16	stretch_h;
		uint8	antialias;
		uint8	padding_up;
		uint8	padding_right;
		uint8	padding_down;
		uint8	padding_left;
		uint8	padding_horiz;
		uint8	padding_vert;
		uint8	outline;
		char	name[1];
	};

	struct CommonBlock
	{
		uint16	line_height;
		uint16	base;
		uint16	scale_width;
		uint16	scale_height;
		uint16	pages;
		uint8	bitfield;
		uint8	flags;
		uint8	chan_alpha;
		uint8	chan_red;
		uint8	chan_green;
		uint8	chan_blue;
	};

	struct PagesBlock
	{
		char names[1];
	};

	struct CharInfo
	{
		uint32	id;
		uint16	x;
		uint16	y;
		uint16	width;
		uint16	height;
		int16	offset_x;
		int16	offset_y;
		int16	advance_x;
		uint8	page;
		uint8	chan;
	};

	struct CharsBlock
	{
		CharInfo chars[1];
	};

	struct KerningPair
	{
		uint32 first;
		uint32 second;
		uint16 amount;
	};

	struct KerningBlock
	{
		KerningPair pairs[1];
	};

#pragma pack(pop)

	static void endian_swap(InfoBlock& x)
	{
		ENDIAN_SWAP(x.font_size);
		ENDIAN_SWAP(x.stretch_h);
	}

	static void endian_swap(CommonBlock& x)
	{
		ENDIAN_SWAP(x.line_height);
		ENDIAN_SWAP(x.base);
		ENDIAN_SWAP(x.scale_width);
		ENDIAN_SWAP(x.scale_height);
		ENDIAN_SWAP(x.pages);
	}

	static void endian_swap(CharInfo& x)
	{
		ENDIAN_SWAP(x.id);
		ENDIAN_SWAP(x.x);
		ENDIAN_SWAP(x.y);
		ENDIAN_SWAP(x.width);
		ENDIAN_SWAP(x.height);
		ENDIAN_SWAP(x.offset_x);
		ENDIAN_SWAP(x.offset_y);
		ENDIAN_SWAP(x.advance_x);
	}

	static void endian_swap(KerningPair& x)
	{
		ENDIAN_SWAP(x.first);
		ENDIAN_SWAP(x.second);
		ENDIAN_SWAP(x.amount);
	}

	bool Font::read( DataStream& stream )
	{	
		uint32 magic;
		stream.read(&magic, 4);

		bool need_endian_swap = false;
		bool magic_found = false;

		uint32 magic_native_endian  = FourCC('B','M','F',3);
		uint32 magic_swapped_endian = magic_native_endian; 
		ENDIAN_SWAP(magic_swapped_endian);

		if( magic == magic_native_endian )
		{
			magic_found = true;
			need_endian_swap = false;
		}
		else if( magic == magic_swapped_endian )
		{
			magic_found = true;
			need_endian_swap = true;
		}

		if( magic_found == false )
		{
			Log::error("Font::read() BMF file identifier not found.");
			return false;
		}

		InfoBlock*		info	= NULL;
		CommonBlock*	common	= NULL;
		PagesBlock*		pages	= NULL;
		CharsBlock*		chars	= NULL;
		KerningBlock*	kerning	= NULL;
		
		uint32 num_chars  = 0;
		uint32 num_kpairs = 0;
		uint32 num_pages  = 0;
		uint32 page_len   = 0;
		
		uint8 type;
		while( stream.read(&type, 1) )
		{
			uint32 block_size;
			stream.read(&block_size, 4);

			if(need_endian_swap) ENDIAN_SWAP(block_size);

			BlockType::type block_type = (BlockType::type )type;

			switch(block_type)
			{
			case BlockType::Info:
				info = (InfoBlock*)malloc(block_size);
				if( info )
				{
					stream.read(info, block_size);
				}
				break;
			case BlockType::Common:
				common = (CommonBlock*)malloc(block_size);
				if( common )
				{
					stream.read(common, block_size);
				}
				break;
			case BlockType::Pages:
				pages = (PagesBlock*)malloc(block_size);
				if( pages )
				{
					stream.read(pages, block_size);
					page_len = (uint32)rush_strlen(pages->names);
					num_pages = block_size / (page_len+1);
				}
				break;
			case BlockType::Chars:
				chars = (CharsBlock*)malloc(block_size);
				if( chars )
				{
					stream.read(chars, block_size);
					num_chars = block_size / sizeof(CharInfo);
				}
				break;
			case BlockType::Kerning:
				kerning = (KerningBlock*)malloc(block_size);
				if( kerning )
				{
					stream.read(kerning, block_size);
					num_kpairs = block_size / sizeof(KerningPair);
				}
				break;
			}
		}

		if( info && pages && chars )
		{
			if( need_endian_swap )
			{
				endian_swap(*info);
				endian_swap(*common);
				for(uint32 i=0; i<num_chars; ++i) endian_swap(chars->chars[i]);
				if( kerning )
				{
					for(uint32 i=0; i<num_kpairs; ++i) endian_swap(kerning->pairs[i]);
				}
			}

			m_size = (uint32)abs((float)info->font_size);

			// handle page info
			m_num_pages = num_pages;
			uint32 clamped_page_len = rush_min<uint32>(page_len, sizeof(PageData)-1);
			for( uint32 i=0; i<m_num_pages; ++i )
			{
				rush_memcpy(m_pages[i].filename, (char*)pages + (page_len+1)*i, clamped_page_len);
				m_pages[i].filename[clamped_page_len] = 0;
			}

			// handle char into
			for( uint32 i=0; i<256; ++i )
			{
				rush_memset(&m_chars[i], 0, sizeof(CharData));
				m_chars[i].page = InvalidPage;
			}

			for( uint32 i=0; i<num_chars; ++i )
			{
				uint32 c = chars->chars[i].id;

				m_chars[c].x         = chars->chars[i].x;
				m_chars[c].y         = chars->chars[i].y;
				m_chars[c].width     = chars->chars[i].width;
				m_chars[c].height    = chars->chars[i].height;
				m_chars[c].offset_x  = chars->chars[i].offset_x;
				m_chars[c].offset_y  = chars->chars[i].offset_y;
				m_chars[c].advance_x = chars->chars[i].advance_x;
				m_chars[c].page      = chars->chars[i].page;
				m_chars[c].chan      = chars->chars[i].chan;
			}
		}
		else
		{
			Log::error("Font::read() Failed to load font data (info, pages or chars).");
		}

		free(info);
		free(common);
		free(pages);
		free(chars);
		free(kerning);

		return true;
	}

	Font::Font() 
		: m_num_pages(0)
		, m_size(0)
	{
		rush_memset(&m_pages, 0, sizeof(m_pages));
	}
}
