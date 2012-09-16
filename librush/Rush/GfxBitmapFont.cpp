#include "GfxBitmapFont.h"

#if RUSH_RENDER_API!=RUSH_RENDER_API_NONE

#include "GfxEmbeddedFont.h"
#include "UtilLog.h"
#include "Filesystem.h"

namespace Rush
{
    BitmapFont::BitmapFont(RenderDevice* device, const BitmapFontData& data)
        : m_device(device)
    {
        Gfx_AddReference(m_device);
        pr_init(data.header_data, data.header_size, data.pixels_data, data.pixels_size, data.width, data.height, data.format);        
		pr_create_character_quads();        
    }
    
	BitmapFont::BitmapFont( RenderDevice* device, const void* header_data, size_t header_size, const void* pixels_data, size_t pixels_size, uint32 width, uint32 height, PixelFormat::type format  )
		: m_device(device)
	{
        Gfx_AddReference(m_device);
        pr_init(header_data, header_size, pixels_data, pixels_size, width, height, format);
		pr_create_character_quads();
	}

	BitmapFont::BitmapFont( RenderDevice* device, const char* font_filename, const char* texture_path )
		: m_device(device)
	{
        Gfx_AddReference(m_device);

		FileIn fi(font_filename);
		if( fi.valid() )
		{
			m_font_data.read(fi);
			for( uint32 i=0; i<m_font_data.num_pages(); ++i )
			{
				const char* page_filename = m_font_data.get_page(i);
				String page_path = texture_path; 
				page_path += "/";
				page_path += page_filename;

				TextureHandle th = Gfx_Texture_Load(m_device, page_path.c_str());
				m_textures.push_back(th);			

				TextureDescr td = Gfx_Texture_Descr(m_device, th);
				m_texture_descr.push_back(td);
			}

			pr_create_character_quads();
		}
		else
		{
			Log::error("Failed to load bitmap font '%s' (texture path '%')", font_filename, texture_path);
		}
	}

	BitmapFont::~BitmapFont()
	{
        for( size_t i=0; i<m_textures.size(); ++i )
        {
            Gfx_Texture_Destroy(m_device, m_textures[i]);
        }
		Gfx_RemoveReference(m_device);
	}
	Vector2 BitmapFont::draw_string( PrimitiveBatch* batch, const Vector2& pos, const char* str, ColourRGBA8 col, bool flush )
	{
		if( flush ) batch->flush();

		if( m_textures.size()==0 ) return pos;

		float x = pos.x;
		float y = pos.y;
		float orig_x = x;
		const TextureDescr& t = Gfx_Texture_Descr(m_device, m_textures[0]);
		Vector2 page_size(float(t.width),float(t.height));
		//Vector2 page_size_rcp = page_size.reciprocal();

		while(*str)
		{
			uint8 c = *str;
			if( c == '\n' )
			{
				y += m_font_data.size();
				x = orig_x;
			}
			else
			{				
				const Font::CharData& data = m_font_data.get_char(c);
				if( data.page!=Font::InvalidPage )
				{
					TexturedQuad2D q = m_chars[c];
					
					q.pos[0] += Vector2(x,y);
					q.pos[1] += Vector2(x,y);
					q.pos[2] += Vector2(x,y);
					q.pos[3] += Vector2(x,y);

					batch->draw_textured_quad(&q, m_textures[data.page], col);
					x += float(data.advance_x);
				}
			}
			++str;
		}

		if( flush ) batch->flush();

		return Vector2(x,y);
	}

	Vector2 BitmapFont::measure_string(const char* str)
	{
		if( m_textures.size()==0 ) return Vector2(0,0);

		float x = 0;
		float y = 0;
		float orig_x = x;
		float max_x  = 0;
		TextureDescr t = Gfx_Texture_Descr(m_device, m_textures[0]);
		Vector2 page_size(float(t.width),float(t.height));
		//Vector2 page_size_rcp = page_size.reciprocal();

		float char_w = 0;
		float char_h = float(m_font_data.size());

		while(*str)
		{
			uint8 c = *str;
			if( c == '\n' )
			{
				max_x = rush_max(max_x, x);
				y += char_h;
				x = orig_x;
			}
			else
			{				
				const Font::CharData& data = m_font_data.get_char(c);
				if( data.page!=Font::InvalidPage )
				{
					char_w = data.advance_x;
					x += float(char_w);
				}
			}
			++str;
		}
		max_x = rush_max(max_x, x);

		return Vector2(max_x, y+char_h);

	}
    
    void BitmapFont::pr_init(const void *header_data, size_t header_size, const void *pixels_data, size_t pixels_size, uint32 width, uint32 height, PixelFormat::type format)
    {
        MemDataStream fi((void*)header_data, (uint32)header_size); // TODO: fix casting away const 
		m_font_data.read(fi);
        
		TextureDescr td = TextureDescr::create_2D(width, height, format, false, false);
		m_texture_descr.push_back(td);
        
		TextureHandle th = Gfx_Texture_Create(m_device, td, pixels_data, pixels_size);
		m_textures.push_back(th);    
    }
    
	void BitmapFont::pr_create_character_quads()
	{
		for( uint8 c=0; c<255; ++c )
		{
			const Font::CharData& data = m_font_data.get_char(c);
			if( data.page!=Font::InvalidPage )
			{
				TextureDescr& texture_descr = m_texture_descr[data.page];
				Vector2 page_size(float(texture_descr.width), float(texture_descr.height));

				float w = float(data.width);
				float h = float(data.height);

				float px = float(data.offset_x);
				float py = float(data.offset_y);

				float tx = float(data.x)		/ page_size.x;
				float ty = float(data.y)		/ page_size.y;
				float tw = float(data.width)	/ page_size.x;
				float th = float(data.height)	/ page_size.y;

				TexturedQuad2D& q = m_chars[c];

				q.pos[0] = Vector2(px,		py);
				q.pos[1] = Vector2(px+w,	py);
				q.pos[2] = Vector2(px+w,	py+h);
				q.pos[3] = Vector2(px,		py+h);

				q.tex[0] = Vector2(tx,		ty);
				q.tex[1] = Vector2(tx+tw,	ty);
				q.tex[2] = Vector2(tx+tw,	ty+th);
				q.tex[3] = Vector2(tx,		ty+th);
			}
		}
	}
    
    BitmapFontData BitmapFont::embedded_font_data()
    {
        BitmapFontData res;
        
        res.header_data = EmbeddedFont::header_bytes;
        res.header_size = EmbeddedFont::header_size;
        res.pixels_data = EmbeddedFont::pixels_bytes;
        res.pixels_size = EmbeddedFont::pixels_size;
        res.width       = EmbeddedFont::width;
        res.height      = EmbeddedFont::height;
        res.format      = PixelFormat::A8R8G8B8;
        
        return res;
    }
}

#endif //RUSH_RENDER_API!=RUSH_RENDER_API_NONE
