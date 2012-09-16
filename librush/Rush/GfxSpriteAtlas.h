#pragma once

#include "Rush.h"
#include "GfxApi.h"

#if RUSH_RENDER_API!=RUSH_RENDER_API_NONE

#include "MathVector2.h"
#include "UtilDynamicArray.h"
#include "GfxCommon.h"

namespace Rush
{
	/// @addtogroup graphics
	/// @{

	class RenderDevice;
	class PrimitiveBatch;
	struct TexturedQuad2D;

	class SpriteAtlas
	{

	public:

		SpriteAtlas(RenderDevice* device, uint32 sprite_width, uint32 sprite_height, const char* filename);
		~SpriteAtlas();

		INLINE uint32 num_sprites() const	{ return m_num_sprites_x * m_num_sprites_y; }
		INLINE const TexturedQuad2D& get_sprite(uint32 n) { return m_sprites[n]; }

		void draw(PrimitiveBatch* batch, const Vector2& pos, uint32 sprite_idx) const;

	private:

		RenderDevice* m_device;

		Vector2 m_sprite_size;
		Vector2 m_texture_size;

		uint32 m_num_sprites_x;
		uint32 m_num_sprites_y;

		TextureHandle m_texture;

		DynamicArray<TexturedQuad2D> m_sprites;
	};

	/// @}
}

#endif //RUSH_RENDER_API!=RUSH_RENDER_API_NONE
