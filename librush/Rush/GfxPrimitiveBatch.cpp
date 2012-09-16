#include "GfxPrimitiveBatch.h"

#if RUSH_RENDER_API!=RUSH_RENDER_API_NONE

#include "GfxVertexFormat.h"
#include "GfxBuffer.h"
#include "UtilAssert.h"
#include "MathMatrix44.h"

#ifdef RUSH_EXTERN_GFX
extern Rush::ShaderCode _rush_primitive_batch_vs_2d;
extern Rush::ShaderCode _rush_primitive_batch_vs_3d;
extern Rush::ShaderCode _rush_primitive_batch_ps_plain;
extern Rush::ShaderCode _rush_primitive_batch_ps_textured;
#endif //RUSH_EXTERN_GFX

namespace Rush
{	
	class BatchVertexFormat : public VertexFormatDescr
	{
	public:
		BatchVertexFormat()
		{
			add( 0, DataType::Float3,  UsageType::Position,	0 );
			add( 0, DataType::Float2,  UsageType::Texcoord,	0 );
			add( 0, DataType::Colour,  UsageType::Colour,	0 );
		}
	};

	PrimitiveBatch::PrimitiveBatch( RenderDevice* device )
		: m_device(device)
		, m_context(Gfx_Immediate_Context(device))
		, m_batch_type(BatchType::None)
		, m_ready_to_draw(false)
	{
        Gfx_AddReference(device);

#ifdef RUSH_EXTERN_GFX
		m_vsh_handle_2d			= Gfx_VertexShader_Create(m_device, _rush_primitive_batch_vs_2d);
		m_vsh_handle_3d			= Gfx_VertexShader_Create(m_device, _rush_primitive_batch_vs_3d);
		m_psh_handle_plain		= Gfx_PixelShader_Create(m_device, _rush_primitive_batch_ps_plain);
		m_psh_handle_textured	= Gfx_PixelShader_Create(m_device, _rush_primitive_batch_ps_textured);
#else //RUSH_EXTERN_GFX
		m_vsh_handle_2d			= Gfx_VertexShader_Create(m_device, ShaderCodeText(pr_source_vs_2d()));
		m_vsh_handle_3d			= Gfx_VertexShader_Create(m_device, ShaderCodeText(pr_source_vs_3d()));
		m_psh_handle_plain		= Gfx_PixelShader_Create(m_device, ShaderCodeText(pr_source_ps_plain()));
		m_psh_handle_textured	= Gfx_PixelShader_Create(m_device, ShaderCodeText(pr_source_ps_textured()));
#endif //RUSH_EXTERN_GFX

		BatchVertexFormat fmt_desc;

		m_vertex_format_2d = Gfx_VertexFormat_Create(m_device, fmt_desc);
		m_vertex_format_3d = Gfx_VertexFormat_Create(m_device, fmt_desc);

		ShaderConstantBindings bindings;
		bindings.add_constant("g_transform",    &m_vsc_transform_2d,    1);
		bindings.add_constant("g_mat_viewproj", &m_vsc_mat_viewproj[0], 4);
		bindings.add_sampler("s_diffuse", 0);

		m_techniques[TechniqueID_Plain_2D]    = Gfx_Technique_Create(m_device, TechniqueDescr(m_psh_handle_plain,    m_vsh_handle_2d, m_vertex_format_2d, &bindings));
		m_techniques[TechniqueID_Plain_3D]    = Gfx_Technique_Create(m_device, TechniqueDescr(m_psh_handle_plain,    m_vsh_handle_3d, m_vertex_format_3d, &bindings));
		m_techniques[TechniqueID_Textured_2D] = Gfx_Technique_Create(m_device, TechniqueDescr(m_psh_handle_textured, m_vsh_handle_2d, m_vertex_format_2d, &bindings));
		m_techniques[TechniqueID_Textured_3D] = Gfx_Technique_Create(m_device, TechniqueDescr(m_psh_handle_textured, m_vsh_handle_3d, m_vertex_format_3d, &bindings));

		m_batch_type_to_primitive_type[(size_t)PrimitiveBatch::BatchType::Line]			= PrimitiveTypes::LineList;
		m_batch_type_to_primitive_type[(size_t)PrimitiveBatch::BatchType::Line3D]		= PrimitiveTypes::LineList;
		m_batch_type_to_primitive_type[(size_t)PrimitiveBatch::BatchType::TriList]		= PrimitiveTypes::TriangleList;
		m_batch_type_to_primitive_type[(size_t)PrimitiveBatch::BatchType::TriList3D]	= PrimitiveTypes::TriangleList;

		VertexBufferDescr vb_descr(BufferMode::WriteOften, MaxBatchVertices, sizeof(BatchVertex));

		m_vertex_buffer = Gfx_VertexBuffer_Create(m_device, vb_descr);

        BatchVertex* vertex_data = new BatchVertex[MaxBatchVertices];
		m_vertices.setup(vertex_data, MaxBatchVertices);
        
        Gfx_VertexBuffer_Update(m_device, m_vertex_buffer, vertex_data, 0, sizeof(BatchVertex)*MaxBatchVertices, true);

		m_sampler_linear = Gfx_SamplerState_Create(m_device, SamplerStateDescr::make_linear());
    }

	PrimitiveBatch::~PrimitiveBatch()
	{
		delete [] m_vertices.data();

		for(uint32 i=0; i<TechniqueID_COUNT; ++i)
		{
			Gfx_Technique_Destroy(m_device, m_techniques[i]);
		}

		Gfx_SamplerState_Destroy(m_device, m_sampler_linear);

		Gfx_VertexBuffer_Destroy(m_device, m_vertex_buffer);

		Gfx_VertexShader_Destroy(m_device, m_vsh_handle_2d);
		Gfx_VertexShader_Destroy(m_device, m_vsh_handle_3d);
		Gfx_PixelShader_Destroy(m_device, m_psh_handle_plain);
		Gfx_PixelShader_Destroy(m_device, m_psh_handle_textured);
		
		Gfx_RemoveReference(m_device);
	}

	void PrimitiveBatch::pr_start_new_batch( BatchType::type type, TextureHandle texture )
	{
		R_ASSERT(m_ready_to_draw);

		if( m_batch_type != type ||
			m_batch_texture != texture ||
			m_vertices.size() == MaxBatchVertices )
		{
			flush();
			m_batch_type = type;
			m_batch_texture = texture;
		}
	}

	TechniqueHandle PrimitiveBatch::pr_next_technique()
	{
        bool enable_textures = true;
        
		if( m_batch_texture.valid() && enable_textures )
		{
			switch(m_batch_type)
			{
			case BatchType::Line3D:
			case BatchType::TriList3D:
				return m_techniques[TechniqueID_Textured_3D];
			default:
				return m_techniques[TechniqueID_Textured_2D];
			}
		}
		else
		{
			switch(m_batch_type)
			{
			case BatchType::Line3D:
			case BatchType::TriList3D:
				return m_techniques[TechniqueID_Plain_3D];
			default:
				return m_techniques[TechniqueID_Plain_2D];
			}
		}
	}

	void PrimitiveBatch::flush()
	{
		R_ASSERT(m_ready_to_draw);

		if( m_vertices.size()==0 ) return;
		if( m_batch_type == BatchType::None ) return;

		TechniqueHandle next_technique = pr_next_technique();

		Gfx_VertexBuffer_Update(m_device, m_vertex_buffer, m_vertices.data(), 0, (uint32)m_vertices.size_in_bytes(), true);

		if( m_batch_texture.valid() )
		{
			Gfx_Context_SetPixelTexture(m_context, 0, m_batch_texture);
			Gfx_Context_SetPixelSamplerState(m_context, 0, m_sampler_linear);
		}

		Gfx_Context_SetTechnique(m_context, next_technique);
		Gfx_Context_SetVertexStream(m_context, 0, m_vertex_buffer);
		Gfx_Context_SetPrimitive(m_context, m_batch_type_to_primitive_type[m_batch_type]);

		Gfx_Context_Draw(m_context, 0, (uint32)m_vertices.size());

		m_vertices.clear();
	}

	void PrimitiveBatch::draw_aabb( const AABB3& box, const ColourRGBA8& colour/*=ColourARGB8::White()*/ )
	{
		Vector3 v[8];

		v[0] = box.center() + box.dimensions() * Vector3(-0.5f,-0.5f,-0.5f);
		v[1] = box.center() + box.dimensions() * Vector3(+0.5f,-0.5f,-0.5f);
		v[2] = box.center() + box.dimensions() * Vector3(-0.5f,+0.5f,-0.5f);
		v[3] = box.center() + box.dimensions() * Vector3(+0.5f,+0.5f,-0.5f);
		v[4] = box.center() + box.dimensions() * Vector3(-0.5f,-0.5f,+0.5f);
		v[5] = box.center() + box.dimensions() * Vector3(+0.5f,-0.5f,+0.5f);
		v[6] = box.center() + box.dimensions() * Vector3(-0.5f,+0.5f,+0.5f);
		v[7] = box.center() + box.dimensions() * Vector3(+0.5f,+0.5f,+0.5f);

		draw_line(Line3(v[0],v[1]), colour);
		draw_line(Line3(v[2],v[3]), colour);
		draw_line(Line3(v[4],v[5]), colour);
		draw_line(Line3(v[6],v[7]), colour);

		draw_line(Line3(v[0],v[4]), colour);
		draw_line(Line3(v[1],v[5]), colour);
		draw_line(Line3(v[2],v[6]), colour);
		draw_line(Line3(v[3],v[7]), colour);

		draw_line(Line3(v[0],v[2]), colour);
		draw_line(Line3(v[1],v[3]), colour);
		draw_line(Line3(v[4],v[6]), colour);
		draw_line(Line3(v[5],v[7]), colour);
	}

	void PrimitiveBatch::draw_triangle( const Vector2& a, const Vector2& b, const Vector2& c, const ColourRGBA8& colour )
	{
		pr_start_new_batch(BatchType::TriList);

		BatchVertex* v = m_vertices.alloc_unsafe(3);

		if( v && m_vertices.data() )
		{
			v[0].pos.x = a.x;
			v[0].pos.y = a.y;
			v[0].pos.z = 0;
			v[0].tex.x = 0;
			v[0].tex.y = 0;
			v[0].col0   = colour;

			v[1].pos.x = b.x;
			v[1].pos.y = b.y;
			v[1].pos.z = 0;
			v[1].tex.x = 0;
			v[1].tex.y = 0;
			v[1].col0   = colour;

			v[2].pos.x = c.x;
			v[2].pos.y = c.y;
			v[2].pos.z = 0;
			v[2].tex.x = 0;
			v[2].tex.y = 0;
			v[2].col0   = colour;
		}
	}

	void PrimitiveBatch::draw_triangle( const Vector3& a, const Vector3& b, const Vector3& c, const ColourRGBA8& colour )
	{
		pr_start_new_batch(BatchType::TriList3D);

		BatchVertex* v = m_vertices.alloc_unsafe(3);

		if( v && m_vertices.data() )
		{
			v[0].pos.x = a.x;
			v[0].pos.y = a.y;
			v[0].pos.z = a.z;
			v[0].tex.x = 0;
			v[0].tex.y = 0;
			v[0].col0   = colour;

			v[1].pos.x = b.x;
			v[1].pos.y = b.y;
			v[1].pos.z = b.z;
			v[1].tex.x = 0;
			v[1].tex.y = 0;
			v[1].col0   = colour;

			v[2].pos.x = c.x;
			v[2].pos.y = c.y;
			v[2].pos.z = c.z;
			v[2].tex.x = 0;
			v[2].tex.y = 0;
			v[2].col0   = colour;
		}
	}

	void PrimitiveBatch::draw_triangle( const Vector3& pa, const Vector3& pb, const Vector3& pc, const ColourRGBA8& ca, const ColourRGBA8& cb, const ColourRGBA8& cc )
	{
		pr_start_new_batch(BatchType::TriList3D);

		BatchVertex* v = m_vertices.alloc_unsafe(3);

		if( v && m_vertices.data() )
		{
			v[0].pos.x = pa.x;
			v[0].pos.y = pa.y;
			v[0].pos.z = pa.z;
			v[0].tex.x = 0;
			v[0].tex.y = 0;
			v[0].col0   = ca;

			v[1].pos.x = pb.x;
			v[1].pos.y = pb.y;
			v[1].pos.z = pb.z;
			v[1].tex.x = 0;
			v[1].tex.y = 0;
			v[1].col0   = cb;

			v[2].pos.x = pc.x;
			v[2].pos.y = pc.y;
			v[2].pos.z = pc.z;
			v[2].tex.x = 0;
			v[2].tex.y = 0;
			v[2].col0   = cc;
		}
	}

	void PrimitiveBatch::begin_2d(const AABB2& bounds)
	{
		R_ASSERT(m_ready_to_draw==false);

		float width  = bounds.width();
		float height = bounds.height();

		m_vsc_transform_2d = Vector4(2.0f/width, -2.0f/height, -1, 1);

		m_ready_to_draw = true;
	}

	void PrimitiveBatch::begin_2d( uint32 width, uint32 height )
	{
		AABB2 bounds(0, 0, float(width), float(height));
		begin_2d(bounds);
	}

	void PrimitiveBatch::begin_3d(const Matrix44& view_proj)
	{
		R_ASSERT(m_ready_to_draw==false);
		Matrix44 mat_transposed = view_proj.transposed();
		m_vsc_mat_viewproj[0] = mat_transposed.rows[0];
		m_vsc_mat_viewproj[1] = mat_transposed.rows[1];
		m_vsc_mat_viewproj[2] = mat_transposed.rows[2];
		m_vsc_mat_viewproj[3] = mat_transposed.rows[3];
		m_ready_to_draw = true;
	}

	void PrimitiveBatch::end_2d()
	{
		R_ASSERT(m_ready_to_draw);
		flush();
		m_ready_to_draw = false;
	}

	void PrimitiveBatch::end_3d()
	{
		R_ASSERT(m_ready_to_draw);
		flush();
		m_ready_to_draw = false;
	}

#ifndef RUSH_EXTERN_GFX
	const char* PrimitiveBatch::pr_source_vs_2d() const
	{
#if RUSH_RENDER_API==RUSH_RENDER_API_DX9
		return 
		"float4 g_transform : register(c0);\n"
		"struct VS_INPUT  { float3 pos : POSITION; float4 col : COLOR0; float2 tex : TEXCOORD0; };\n"
		"struct VS_OUTPUT { float4 pos : POSITION; float4 col : COLOR0; float2 tex : TEXCOORD0; };\n"
		"VS_OUTPUT main(VS_INPUT v)\n"
		"{\n"
		"	VS_OUTPUT r;\n"
		"	r.pos.xy = (v.pos.xy-0.5f) * g_transform.xy + g_transform.zw;\n"
		"	r.pos.zw = float2(0,1);\n"
		"	r.col = v.col.rgba;\n"
		"	r.tex = v.tex;\n"
		"	return r;\n"
		"}\n";
#elif RUSH_RENDER_API==RUSH_RENDER_API_DX11
		return
		"float4 g_transform : register(c0);\n"
		"struct VS_INPUT  { float3 pos : POSITION; float4 col : COLOR0; float2 tex : TEXCOORD0; };\n"
		"struct VS_OUTPUT { float4 pos : SV_POSITION; float4 col : COLOR0; float2 tex : TEXCOORD0; };\n"
		"VS_OUTPUT main(VS_INPUT v)\n"
		"{\n"
		"	VS_OUTPUT r;\n"
		"	r.pos.xy = v.pos.xy * g_transform.xy + g_transform.zw;\n"
		"	r.pos.zw = float2(0,1);\n"
		"	r.col = v.col.rgba;\n"
		"	r.tex = v.tex;\n"
		"	return r;\n"
		"}\n";
#elif RUSH_RENDER_API==RUSH_RENDER_API_GLES2 || RUSH_RENDER_API==RUSH_RENDER_API_GL
		return 
		"attribute vec3 a_pos0;\n"
		"attribute vec2 a_tex0;\n"
		"attribute vec4 a_col0;\n"
		"uniform vec4 g_transform;\n"
		"varying vec4 v_col0;\n"
		"varying vec2 v_tex0;\n"
		"void main()\n"
		"{\n"
		"	gl_Position.xy = a_pos0.xy * g_transform.xy + g_transform.zw;\n"
		"	gl_Position.zw = vec2(0, 1);\n"
		"	v_col0 = a_col0;\n"
		"	v_tex0 = a_tex0;\n"
		"}\n";
#else
		return "";
#endif
	}

	const char* PrimitiveBatch::pr_source_vs_3d() const
	{
#if RUSH_RENDER_API==RUSH_RENDER_API_DX9
		return
		"float4x4 g_mat_viewproj : register(c1);\n"
		"struct VS_INPUT  { float3 pos : POSITION; float4 col : COLOR0; float2 tex : TEXCOORD0; };\n"
		"struct VS_OUTPUT { float4 pos : POSITION; float4 col : COLOR0; float2 tex : TEXCOORD0; };\n"
		"VS_OUTPUT main(VS_INPUT v)\n"
		"{\n"
		"	VS_OUTPUT r;\n"
		"	r.pos = mul(float4(v.pos,1), g_mat_viewproj);\n"
		"	r.col = v.col.rgba;\n"
		"	r.tex = v.tex;\n"
		"	return r;\n"
		"}\n";
#elif RUSH_RENDER_API==RUSH_RENDER_API_DX11
		return
		"float4x4 g_mat_viewproj : register(c1);\n"
		"struct VS_INPUT  { float3 pos : POSITION; float4 col : COLOR0; float2 tex : TEXCOORD0; };\n"
		"struct VS_OUTPUT { float4 pos : SV_POSITION; float4 col : COLOR0; float2 tex : TEXCOORD0; };\n"
		"VS_OUTPUT main(VS_INPUT v)\n"
		"{\n"
		"	VS_OUTPUT r;\n"
		"	r.pos = mul(float4(v.pos,1), g_mat_viewproj);\n"
		"	r.col = v.col.rgba;\n"
		"	r.tex = v.tex;\n"
		"	return r;\n"
		"}\n";
#elif RUSH_RENDER_API==RUSH_RENDER_API_GLES2 || RUSH_RENDER_API==RUSH_RENDER_API_GL
		return
		"attribute vec3 a_pos0;\n"
		"attribute vec2 a_tex0;\n"
		"attribute vec4 a_col0;\n"
		"uniform mat4 g_mat_viewproj;\n"
		"varying vec4 v_col0;\n"
		"varying vec2 v_tex0;\n"
		"void main()\n"
		"{\n"
        "	gl_Position = vec4(a_pos0, 1) * g_mat_viewproj;\n"
		"	v_col0 = a_col0;\n"
		"	v_tex0 = a_tex0;\n"
		"}\n";
#else
		return "";
#endif
	}

	const char* PrimitiveBatch::pr_source_ps_plain() const
	{
#if RUSH_RENDER_API==RUSH_RENDER_API_DX9
		return 
		"struct VS_OUTPUT { float4 pos : POSITION; float4 col : COLOR0; float2 tex : TEXCOORD0; };\n"
		"float4 main(VS_OUTPUT v) : COLOR\n"
		"{\n"
		"	return v.col;\n"
		"}\n";
#elif RUSH_RENDER_API==RUSH_RENDER_API_DX11
		return
		"struct VS_OUTPUT { float4 pos : POSITION; float4 col : COLOR0; float2 tex : TEXCOORD0; };\n"
		"float4 main(VS_OUTPUT v) : SV_TARGET\n"
		"{\n"
		"	return v.col;\n"
		"}\n";
#elif RUSH_RENDER_API==RUSH_RENDER_API_GLES2 || RUSH_RENDER_API==RUSH_RENDER_API_GL
		return
        #if RUSH_RENDER_API==RUSH_RENDER_API_GLES2
		"precision lowp float;\n"
        #endif
		"varying vec4 v_col0;\n"
		"void main()\n"
		"{\n"
		"	gl_FragColor = v_col0;\n"
		"}\n";
#else
		return "";
#endif
	}

	const char* PrimitiveBatch::pr_source_ps_textured() const
	{
#if RUSH_RENDER_API==RUSH_RENDER_API_DX9
		return 
		"sampler2D s_diffuse : register(s0);\n"
		"struct VS_OUTPUT { float4 pos : POSITION; float4 col : COLOR0; float2 tex : TEXCOORD0; };\n"
		"float4 main(VS_OUTPUT v) : COLOR\n"
		"{\n"
		"	return v.col * tex2D(s_diffuse, v.tex);\n"
		"}\n";
#elif RUSH_RENDER_API==RUSH_RENDER_API_DX11
		return
		"Texture2D t_diffuse : register(t0);\n"
		"SamplerState s_diffuse : register(s0);\n"
		"struct VS_OUTPUT { float4 pos : SV_POSITION; float4 col : COLOR0; float2 tex : TEXCOORD0; };\n"
		"float4 main(VS_OUTPUT v) : SV_TARGET\n"
		"{\n"
		"	return v.col * t_diffuse.Sample(s_diffuse, v.tex);\n"
		"}\n";
#elif RUSH_RENDER_API==RUSH_RENDER_API_GLES2 || RUSH_RENDER_API==RUSH_RENDER_API_GL
		return
        #if RUSH_RENDER_API==RUSH_RENDER_API_GLES2
		"precision lowp float;\n"
        #endif
		"varying vec4 v_col0;\n"
		"varying vec2 v_tex0;\n"
		"uniform sampler2D s_diffuse;\n"
		"void main()\n"
		"{\n"
		"	gl_FragColor = texture2D(s_diffuse, v_tex0) * v_col0;\n"
		"}\n";
#else
		return "";
#endif
	}

#endif //RUSH_EXTERN_GFX

}

#endif //RUSH_RENDER_API!=RUSH_RENDER_API_NONE
