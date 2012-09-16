#include "GfxSpriteAtlas.h"

#if RUSH_RENDER_API!=RUSH_RENDER_API_NONE

#include "GfxDevice.h"
#include "GfxTexture.h"
#include "GfxPrimitiveBatch.h"

namespace Rush
{
	SpriteAtlas::SpriteAtlas( RenderDevice* device, uint32 sprite_width, uint32 sprite_height, const char* filename ) 
		: m_device(device)		
		, m_sprite_size(float(sprite_width), float(sprite_height))
		, m_texture_size(1,1)
		, m_num_sprites_x(0)
		, m_num_sprites_y(0)
	{
        Gfx_AddReference(m_device);

		m_texture = Gfx_Texture_Load(m_device, filename);

		TextureDescr descr = Gfx_Texture_Descr(m_device, m_texture);

		m_texture_size.x = float(descr.width);
		m_texture_size.y = float(descr.height);

		m_num_sprites_x = descr.width  / sprite_width;
		m_num_sprites_y = descr.height / sprite_height;

		m_sprites.resize(num_sprites());

		for(uint32 y=0; y<m_num_sprites_y; ++y)
		{
			for(uint32 x=0; x<m_num_sprites_x; ++x)
			{
				TexturedQuad2D& q = m_sprites[x + y*m_num_sprites_x];

				float w = m_sprite_size.x;
				float h = m_sprite_size.y;

				float tx = float(x)*m_sprite_size.x	/ m_texture_size.x;
				float ty = float(y)*m_sprite_size.x	/ m_texture_size.y;

				float tw = m_sprite_size.x / m_texture_size.x;
				float th = m_sprite_size.y / m_texture_size.y;

				q.pos[0] = Vector2(0,	0);
				q.pos[1] = Vector2(w,	0);
				q.pos[2] = Vector2(w,	h);
				q.pos[3] = Vector2(0,	h);

				q.tex[0] = Vector2(tx,		ty);
				q.tex[1] = Vector2(tx+tw,	ty);
				q.tex[2] = Vector2(tx+tw,	ty+th);
				q.tex[3] = Vector2(tx,		ty+th);
			}
		}
	}

	SpriteAtlas::~SpriteAtlas()
	{
        Gfx_Texture_Destroy(m_device, m_texture);
		Gfx_RemoveReference(m_device);
	}

	void SpriteAtlas::draw( PrimitiveBatch* batch, const Vector2& pos, uint32 sprite_idx ) const
	{
		if( sprite_idx < num_sprites() )
		{
			TexturedQuad2D q = m_sprites[sprite_idx];

			q.pos[0] += pos;
			q.pos[1] += pos;
			q.pos[2] += pos;
			q.pos[3] += pos;

			batch->draw_textured_quad(&q, m_texture);
		}
	}

}

#endif //RUSH_RENDER_API!=RUSH_RENDER_API_NONE
