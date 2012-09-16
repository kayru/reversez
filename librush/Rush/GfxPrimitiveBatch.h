#pragma once

#include "GfxApi.h"

#if RUSH_RENDER_API!=RUSH_RENDER_API_NONE

#include "MathLine.h"
#include "MathPixel.h"
#include "MathAABB3.h"
#include "UtilStackAllocator.h"
#include "UtilStaticArray.h"
#include "GfxDevice.h"
#include "GfxShader.h"

namespace Rush
{
	struct TexturedQuad2D
	{
		Vector2 pos[4];
		Vector2 tex[4];
	};

	class PrimitiveBatch
	{
	public:
		
		enum
		{ 
			MaxBatchVertices = 12000 // must divide by 6
		};
        
		struct BatchType
		{
			enum type 
			{
				None,
				Line,
				Line3D,
				TriList,
				TriList3D,

				_num_elements
			};
		};

		struct BatchVertex
		{
			Vector3		pos;
			Vector2		tex;
			ColourRGBA8 col0;
		};

	public:

		PrimitiveBatch(RenderDevice* device);
		~PrimitiveBatch();

		Vector4 get_transform_2d() { return m_vsc_transform_2d; }

		void begin_2d(uint32 width, uint32 height);
		void begin_2d(const AABB2& bounds);
		void end_2d();

		void begin_3d(const Matrix44& view_proj);
		void end_3d();

		void draw_line(const Line2& line, const ColourRGBA8& colour=ColourRGBA8::White());
		void draw_line(const Line2& line, const ColourRGBA8& colour_start, const ColourRGBA8& colour_end);
		void draw_line(const Line3& line, const ColourRGBA8& colour=ColourRGBA8::White());
		void draw_line(const Line3& line, const ColourRGBA8& colour_start, const ColourRGBA8& colour_end);
		void draw_rect(const AABB2& rect, const ColourRGBA8& colour=ColourRGBA8::White());

		void draw_triangle( const Vector2& a, const Vector2& b, const Vector2& c, const ColourRGBA8& colour );
		void draw_triangle( const Vector3& a, const Vector3& b, const Vector3& c, const ColourRGBA8& colour );
		void draw_triangle( const Vector3& pa, const Vector3& pb, const Vector3& pc, const ColourRGBA8& ca, const ColourRGBA8& cb, const ColourRGBA8& cc);

		void draw_textured_quad(const AABB2& rect, TextureHandle tex, ColourRGBA8 colour=ColourRGBA8::White());
		void draw_textured_quad(const TexturedQuad2D* q, TextureHandle tex, ColourRGBA8 colour=ColourRGBA8::White());

		void draw_textured_quad(
			const Vector2& a, const Vector2& b, 
			const Vector2& c, const Vector2& d, 
			TextureHandle tex, ColourRGBA8 colour=ColourRGBA8::White(),
			const Vector2& ta=Vector2(0,0), const Vector2& tb=Vector2(1,0), 
			const Vector2& tc=Vector2(1,1), const Vector2& td=Vector2(0,1));

		void draw_cross(const Vector3& pos, float size, const ColourRGBA8& colour=ColourRGBA8::White());
		void draw_aabb(const AABB3& box, const ColourRGBA8& colour=ColourRGBA8::White());

		void flush();
        
	private:

		TechniqueHandle pr_next_technique();

		void pr_start_new_batch(BatchType::type type, TextureHandle texture=InvalidResourceHandle());

#ifndef RUSH_EXTERN_GFX
		const char* pr_source_vs_2d() const;
		const char* pr_source_vs_3d() const;
		const char* pr_source_ps_plain() const;
		const char* pr_source_ps_textured() const;
#endif //RUSH_EXTERN_GFX

	private:

		RenderDevice*		m_device;
		RenderContext*		m_context;
		
		BatchType::type		m_batch_type;
		TextureHandle		m_batch_texture;

		SamplerStateHandle	m_sampler_linear;

		VertexBufferHandle			m_vertex_buffer;
		StackAllocator<BatchVertex> m_vertices;

		StaticArray<PrimitiveType, PrimitiveBatch::BatchType::_num_elements> m_batch_type_to_primitive_type;

		bool m_ready_to_draw;

		VertexFormatHandle		m_vertex_format_2d;
		VertexFormatHandle		m_vertex_format_3d;

		Vector4					m_vsc_transform_2d;
		Vector4					m_vsc_mat_viewproj[4];

		VertexShaderHandle		m_vsh_handle_2d;
		VertexShaderHandle		m_vsh_handle_3d;

		PixelShaderHandle		m_psh_handle_plain;
		PixelShaderHandle		m_psh_handle_textured;

		enum TechniqueID
		{			
			TechniqueID_Plain_2D,
			TechniqueID_Plain_3D,
			TechniqueID_Textured_2D,
			TechniqueID_Textured_3D,
			TechniqueID_COUNT
		};

		TechniqueHandle m_techniques[TechniqueID_COUNT];        
	};

}

#include "GfxPrimitiveBatch.inl.h"

#endif //RUSH_RENDER_API!=RUSH_RENDER_API_NONE

