#pragma once

#include "Rush.h"
#include "GfxApi.h"

#if RUSH_RENDER_API!=RUSH_RENDER_API_NONE

#include "UtilFont.h"
#include "GfxCommon.h"
#include "GfxPrimitiveBatch.h"

namespace Rush
{
	/// @addtogroup graphics
	/// @{
    
    struct BitmapFontData
    {
        const void* header_data;
        size_t header_size;
        const void* pixels_data;
        size_t pixels_size;
        uint32 width;
        uint32 height;
        PixelFormat::type format;
    };

	class BitmapFont
	{
	public:

        BitmapFont(RenderDevice* device, const BitmapFontData& data);
		BitmapFont(RenderDevice* device, const void* header_data, size_t header_size, const void* pixels_data, size_t pixels_size, uint32 width, uint32 height, PixelFormat::type format);
		BitmapFont(RenderDevice* device, const char* font_filename, const char* texture_path);
		~BitmapFont();

		// returns position after the last drawn character
		Vector2 draw_string(PrimitiveBatch* batch, const Vector2& pos, const char* str, ColourRGBA8 col=ColourRGBA8::White(), bool flush=true);

		// returns the width and height of the text block in pixels
		Vector2 measure_string(const char* str);

        static BitmapFontData embedded_font_data();

	private:
        
        void pr_init(const void* header_data, size_t header_size, const void* pixels_data, size_t pixels_size, uint32 width, uint32 height, PixelFormat::type format);
		void pr_create_character_quads();

	private:

		RenderDevice* m_device;
		Font m_font_data;
		DynamicArray<TextureHandle> m_textures;
		DynamicArray<TextureDescr> m_texture_descr;
		TexturedQuad2D m_chars[256];

	};

	/// @}

}

#endif //RUSH_RENDER_API!=RUSH_RENDER_API_NONE

