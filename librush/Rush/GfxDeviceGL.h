#pragma once

#include "GfxApi.h"

#if RUSH_RENDER_API==RUSH_RENDER_API_GL

namespace Rush
{

#if 0

	struct RenderDeviceGLPrivate;
	struct RenderResourcesGL;

	class RenderContextGL : public RenderContextBase
	{
	public:

		RenderContextGL();

		void init(RenderDeviceStats* stats, RenderResourcesGL* resources);

		void draw(uint32 first_vertex, uint32 vertex_count);
		void draw_indexed(uint32 index_count, uint32 first_index, uint32 base_vertex, uint32 num_vertices);

		void commit();

	private:

		RenderResourcesGL* m_resources;
		DynamicArray<uint32> m_enabled_vertex_attributes;
		uint32 m_primitive_type;
		uint32 m_index_type;
		uint32 m_index_size;
        RenderDeviceStats* m_stats;
	};

	class RenderContext : public RenderContextGL {};

	class RenderDeviceGL : public ReferenceCount
	{
        friend class RenderContextGL;

	public:

		RenderDeviceGL(Rush::Window* window, const RenderDeviceConfig& cfg);
		~RenderDeviceGL();

		inline void*					native_device() { return NULL; }
		inline RenderContext*			default_context() { return &m_default_context; }

		void							frame_begin();
		void							frame_end();

		void							clear(ColourRGBA8 colour, ClearFlags::type clear_flags = ClearFlags::Default, float depth=1.0f, uint32 stencil=0);
		void 							present();

		//////////////////////////////////////////////////////////////////////////

		VertexFormatHandle				create_vertex_format(const VertexFormatDescr& fmt);
		VertexShaderHandle				create_vertex_shader(const ShaderCode& code);
		PixelShaderHandle				create_pixel_shader(const ShaderCode& code);
		TextureHandle					create_texture(const TextureDescr& tex, const void* data=NULL, size_t size=0);
		TextureHandle					load_texture(const char* filename, TextureType::type type=TextureType::Tex2D);
		BlendStateHandle				create_blend_state(const BlendStateDescr& descr);
		SamplerStateHandle				create_sampler_state(const SamplerStateDescr& descr);
		IndexBufferHandle				create_index_buffer(const IndexBufferDescr& ib, const void* data=NULL);
		VertexBufferHandle				create_vertex_buffer(const VertexBufferDescr& vb, const void* data=NULL);
		TextureHandle					create_render_target(const TextureDescr& tex);
		TechniqueHandle					create_technique(const TechniqueDescr& descr);

		void							destroy_index_buffer(IndexBufferHandle ib);
		void							destroy_vertex_buffer(VertexBufferHandle vb);
		void							destroy_vertex_shader(VertexShaderHandle& vsh);
		void							destroy_pixel_shader(PixelShaderHandle& psh);
		void							destroy_texture(TextureHandle& th);
		void							destroy_blend_state(BlendStateHandle h);
		void							destroy_sampler_state(SamplerStateHandle h);
		void							destroy_technique(TechniqueHandle h);

		IndexBufferLock					lock_index_buffer(IndexBufferHandle ib, uint32 offset, uint32 size);   // offset and size in bytes
		VertexBufferLock				lock_vertex_buffer(VertexBufferHandle vb, uint32 offset, uint32 size); // offset and size in bytes
		void							unlock_index_buffer(IndexBufferLock& lock);
		void							unlock_vertex_buffer(VertexBufferLock& lock);

		void							update_index_buffer(IndexBufferHandle h, void* data, uint32 offset=0, uint32 size=0, bool discard=false);
		void							update_vertex_buffer(VertexBufferHandle h, void* data, uint32 offset=0, uint32 size=0, bool discard=false);

		const TextureDescr&				texture_descr(TextureHandle h) const;

        const RenderDeviceStats&        stats();
        void                            reset_stats();
        
	private:

		Window*							m_window;
		RenderContext					m_default_context;
		RenderDeviceGLPrivate*			m_private;
		RenderResourcesGL*				m_resources;
        RenderDeviceStats               m_stats;

	};

	typedef RenderDeviceGL NativeRenderDevice;
#endif //0

}

#endif //RUSH_RENDER_API==RUSH_RENDER_API_GL
