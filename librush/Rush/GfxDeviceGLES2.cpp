#include "GfxDeviceGLES2.h"

#if RUSH_RENDER_API==RUSH_RENDER_API_GLES2

#include "GfxDevice.h"
#include "GfxBuffer.h"
#include "GfxVertexFormat.h"
#include "GfxShader.h"

#include "Window.h"
#include "UtilLog.h"
#include "UtilIndexedPool.h"
#include "UtilImage.h"

#ifdef RUSH_PLATFORM_IOS

#include <OpenGLES/ES1/gl.h>
#include <OpenGLES/ES1/glext.h>
#include <OpenGLES/ES2/gl.h>
#include <OpenGLES/ES2/glext.h>

#else // RUSH_PLATFORM_IOS

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <EGL/egl.h>

#include <malloc.h>
#include <stdlib.h>

#endif // RUSH_PLATFORM_IOS

namespace Rush
{

	inline void* gl_buffer_offset(uint32 offset)
	{
		return ((char*)(NULL)) + offset;
	}

	inline GLenum gles2_convert(BufferMode::type usage)
	{
		switch(usage)
		{
		case BufferMode::WriteOnce:		return GL_STATIC_DRAW;
		case BufferMode::WriteOften:	return GL_STREAM_DRAW;
		case BufferMode::WriteRarely:	return GL_DYNAMIC_DRAW;
		default:						return GL_DYNAMIC_DRAW;
		}
	}

	static inline GLenum gles2_convert(BlendOp::type op)
	{
		switch(op)
		{
		case BlendOp::Add:			return GL_FUNC_ADD;
		case BlendOp::Subtract:		return GL_FUNC_SUBTRACT;
		case BlendOp::RevSubtract:	return GL_FUNC_REVERSE_SUBTRACT;
		case BlendOp::Min:			return GL_MIN_EXT;
		case BlendOp::Max:			return GL_MAX_EXT;
		default:					return 0;
		}
	}

	static inline GLenum gles2_convert(Blend::type blend)
	{
		switch(blend)
		{
		case Blend::Zero:				return GL_ZERO;
		case Blend::One:				return GL_ONE;
		case Blend::SrcColour:			return GL_SRC_COLOR;
		case Blend::InvSrcColour:		return GL_ONE_MINUS_SRC_COLOR;
		case Blend::SrcAlpha:			return GL_SRC_ALPHA;
		case Blend::InvSrcAlpha:		return GL_ONE_MINUS_SRC_ALPHA;
		case Blend::DestAlpha:			return GL_DST_ALPHA;
		case Blend::InvDestAlpha:		return GL_ONE_MINUS_DST_ALPHA;
		case Blend::DestColour:			return GL_DST_COLOR;
		case Blend::InvDestColour:		return GL_ONE_MINUS_DST_COLOR;
		default:						return 0;
		}
	}

	static GLint vertex_data_type_to_count( VertexFormatDescr::DataType::type type )
	{
		switch(type)
		{
		case VertexFormatDescr::DataType::Float1:		return 1;
		case VertexFormatDescr::DataType::Float2:		return 2;
		case VertexFormatDescr::DataType::Float3:		return 3;
		case VertexFormatDescr::DataType::Float4:		return 4;
		case VertexFormatDescr::DataType::Half2:		return 2;
		case VertexFormatDescr::DataType::Half4:		return 4;
		case VertexFormatDescr::DataType::Short2:		return 2;
		case VertexFormatDescr::DataType::Short2N:		return 2;
		case VertexFormatDescr::DataType::UByte4:		return 4;
		case VertexFormatDescr::DataType::Dec3N:		return 3;
		case VertexFormatDescr::DataType::Colour:		return 4;
		default:										return 0;
		}
	}

	static GLenum vertex_data_type_to_gles2( VertexFormatDescr::DataType::type type )
	{
		switch(type)
		{
		case VertexFormatDescr::DataType::Float1:		return GL_FLOAT;
		case VertexFormatDescr::DataType::Float2:		return GL_FLOAT;
		case VertexFormatDescr::DataType::Float3:		return GL_FLOAT;
		case VertexFormatDescr::DataType::Float4:		return GL_FLOAT;
		case VertexFormatDescr::DataType::Half2:		return 0; // not supported
		case VertexFormatDescr::DataType::Half4:		return 0; // not supported
		case VertexFormatDescr::DataType::Short2:		return GL_SHORT;
		case VertexFormatDescr::DataType::Short2N:		return GL_SHORT;
		case VertexFormatDescr::DataType::UByte4:		return GL_UNSIGNED_BYTE;
		case VertexFormatDescr::DataType::Dec3N:		return 0; // not supported
		case VertexFormatDescr::DataType::Colour:		return GL_UNSIGNED_BYTE;
		default:										return 0;
		}
	}

	static GLboolean vertex_data_type_to_normalized( VertexFormatDescr::DataType::type type )
	{
		switch(type)
		{
		case VertexFormatDescr::DataType::Float1:		return GL_FALSE;
		case VertexFormatDescr::DataType::Float2:		return GL_FALSE;
		case VertexFormatDescr::DataType::Float3:		return GL_FALSE;
		case VertexFormatDescr::DataType::Float4:		return GL_FALSE;
		case VertexFormatDescr::DataType::Half2:		return GL_FALSE;
		case VertexFormatDescr::DataType::Half4:		return GL_FALSE; 
		case VertexFormatDescr::DataType::Short2:		return GL_FALSE;
		case VertexFormatDescr::DataType::Short2N:		return GL_TRUE;
		case VertexFormatDescr::DataType::UByte4:		return GL_FALSE;
		case VertexFormatDescr::DataType::Dec3N:		return GL_TRUE;
		case VertexFormatDescr::DataType::Colour:		return GL_TRUE;
		default:										return GL_FALSE;
		}
	}
    
	struct VertexFormatGLES2
	{
		VertexFormatDescr descr;
	};

	struct VertexShaderGLES2
	{
		GLuint native;
	};

	struct PixelShaderGLES2
	{
		GLuint native;
	};

	struct TextureGLES2
	{
		TextureDescr descr;
		GLuint native;
	};

	struct IndexBufferGLES2
	{
		IndexBufferDescr descr;
		GLuint native;
	};

	struct VertexBufferGLES2
	{
		VertexBufferDescr descr;
		GLuint native;
	};

	struct SamplerStateGLES2
	{
		SamplerStateDescr descr;
	};

	struct BlendStateGLES2
	{
		BlendStateDescr descr;
	};

	struct TechniqueGLES2
	{
		struct UniformBinding : public ShaderConstantIndex
		{
			const Vector4* data;
		};

		struct SamplerBinding
		{
			uint32 index;
			GLint location;
		};

		struct AttributeBinding
		{
			uint16 stream;
			GLint location;
			GLint count;
			GLenum type;
			GLboolean normalized;
			GLsizei stride;
			uint16 offset;
		};

		GLuint native;
        GLuint vertex_array_object;
        
		DynamicArray<UniformBinding> uniforms;
		DynamicArray<AttributeBinding> attributes;
		DynamicArray<SamplerBinding> samplers;

	};

	struct RenderResourcesGLES2
	{
		ResourcePool<VertexFormatGLES2,	    VertexFormatHandle, 	GfxMaxVertexFormats>	vertex_formats;
		ResourcePool<VertexShaderGLES2,	    VertexShaderHandle, 	GfxMaxVertexShaders>	vertex_shaders;
		ResourcePool<PixelShaderGLES2,		PixelShaderHandle, 		GfxMaxPixelShaders>		pixel_shaders;
		ResourcePool<TextureGLES2,			TextureHandle, 			GfxMaxTextures>			textures;
		ResourcePool<IndexBufferGLES2,		IndexBufferHandle, 		GfxMaxIndexBuffers>		index_buffers;
		ResourcePool<VertexBufferGLES2,	    VertexBufferHandle, 	GfxMaxVertexBuffers>	vertex_buffers;
		ResourcePool<SamplerStateGLES2,	    SamplerStateHandle, 	GfxMaxSamplerStates>	sampler_states;
		ResourcePool<BlendStateGLES2,		BlendStateHandle, 		GfxMaxBlendStates>		blend_states;
		ResourcePool<TechniqueGLES2,		TechniqueHandle, 		GfxMaxTechniques>		techniques;
	};

	//////////////////////////////////////////////////////////////////////////

	static GLuint gles2_compile_shader(const char* source, ShaderType::type type)
	{
		// copypaste from 
		GLuint shader;
		GLint compiled;

		GLenum gl_shader_type = (type==ShaderType::Pixel) ? GL_FRAGMENT_SHADER : GL_VERTEX_SHADER ;

		// Create the shader object
		shader = glCreateShader ( gl_shader_type );

		if ( shader == 0 )
			return 0;

		// Load the shader source
		glShaderSource ( shader, 1, &source, NULL );

		// Compile the shader
		glCompileShader ( shader );

		// Check the compile status
		glGetShaderiv ( shader, GL_COMPILE_STATUS, &compiled );

		if ( !compiled ) 
		{
			GLint infoLen = 0;

			glGetShaderiv ( shader, GL_INFO_LOG_LENGTH, &infoLen );

			if ( infoLen > 1 )
			{
				char* infoLog = (char*)malloc (sizeof(char) * infoLen );

				glGetShaderInfoLog ( shader, infoLen, NULL, infoLog );
				Log::error( "Error compiling shader:\n%s\n", infoLog );

				free ( infoLog );
			}

			glDeleteShader ( shader );
			return 0;
		}

		return shader;
	}

    inline const char* usage_type_to_string(VertexFormatDescr::UsageType::type type)
    {		
        switch(type)
        {			
        case VertexFormatDescr::UsageType::Position:	return "a_pos";
        case VertexFormatDescr::UsageType::Normal:		return "a_nor";
        case VertexFormatDescr::UsageType::Binormal:	return "a_bin";
        case VertexFormatDescr::UsageType::Tangent:		return "a_tan";
        case VertexFormatDescr::UsageType::Colour:		return "a_col";
        case VertexFormatDescr::UsageType::Texcoord:	return "a_tex";
        }
        return "UNUSED";
    }

#if 0

	//////////////////////////////////////////////////////////////////////////

	struct RenderDeviceGLES2Private
	{
#ifdef WIN32
		HWND hwnd;
		HDC hdc;
#endif //WIN32

#ifdef RUSH_PLATFORM_IOS
        //EGLDisplay egl_display;
		//EGLContext egl_context;
		//EGLSurface egl_surface;
#else // RUSH_PLATFORM_IOS
		EGLDisplay egl_display;
		EGLContext egl_context;
		EGLSurface egl_surface;
#endif //RUSH_PLATFORM_IOS
	};

	RenderDeviceGLES2::RenderDeviceGLES2( Rush::Window* window, const RenderDeviceConfig& cfg )
		: m_window(window)
		, m_private(NULL)
		, m_resources(NULL)
	{
        reset_stats();
        
        m_resources = new RenderResourcesGLES2;
		m_private   = new RenderDeviceGLES2Private;
        
		rush_memset(m_private, 0, sizeof(RenderDeviceGLES2Private));
        
		m_default_context.init(&m_stats, m_resources);
                
#ifdef WIN32		

		EGLint num_configs;
		EGLint major_version;
		EGLint minor_version;
		EGLDisplay display;
		EGLContext context;
		EGLSurface surface;
		EGLConfig config;
		EGLint context_attribs[] = { EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE, EGL_NONE };

		EGLint attrib_list[] =
		{
			EGL_RED_SIZE,       5,
			EGL_GREEN_SIZE,     6,
			EGL_BLUE_SIZE,      5,
			EGL_ALPHA_SIZE,     EGL_DONT_CARE,
			EGL_DEPTH_SIZE,     EGL_DONT_CARE,
			EGL_STENCIL_SIZE,   EGL_DONT_CARE,
			EGL_SAMPLE_BUFFERS, 0,
			EGL_NONE
		};

		EGLBoolean code = EGL_FALSE;

		m_private->hwnd = *((HWND*)m_window->native_window_handle());
		m_private->hdc = GetDC(m_private->hwnd);

		// Get Display
		display = eglGetDisplay(m_private->hdc);
		R_ASSERT( display != EGL_NO_DISPLAY );

		// Initialize EGL
		code = eglInitialize(display, &major_version, &minor_version);
		R_ASSERT( code != EGL_FALSE );

		// Get configs
		code = eglGetConfigs(display, NULL, 0, &num_configs);
		R_ASSERT( code != EGL_FALSE );

		// Choose config
		code = eglChooseConfig(display, attrib_list, &config, 1, &num_configs);
		R_ASSERT( code != EGL_FALSE );

		// Create a surface
		surface = eglCreateWindowSurface(display, config, (EGLNativeWindowType)m_private->hwnd, NULL);
		R_ASSERT( surface != EGL_NO_SURFACE );

		// Create a GL context
		context = eglCreateContext(display, config, EGL_NO_CONTEXT, context_attribs );
		R_ASSERT( context != EGL_NO_CONTEXT );

		// Make the context current
		code = eglMakeCurrent(display, surface, surface, context);
		R_ASSERT( code != EGL_FALSE );

		m_private->egl_display = display;
		m_private->egl_surface = surface;
		m_private->egl_context = context;
#endif //WIN32		

	}

	RenderDeviceGLES2::~RenderDeviceGLES2()
	{
		// todo: cleanup GLES2

		delete m_private;
		delete m_resources;
	}

	void RenderDeviceGLES2::frame_begin()
	{
	}

	void RenderDeviceGLES2::frame_end()
	{	
	}

	void RenderDeviceGLES2::clear( ColourRGBA8 colour, ClearFlags::type clear_flags, float depth, uint32 stencil )
	{
		ColourRGBA colour_fp = colour;
		GLbitfield clear_bits = 0;
		if( clear_flags & ClearFlags::Colour  )
		{
			clear_bits |= GL_COLOR_BUFFER_BIT;
			glClearColor(colour_fp.r, colour_fp.g, colour_fp.b, colour_fp.a);
		}
		if( clear_flags & ClearFlags::Depth   )
		{
			clear_bits |= GL_DEPTH_BUFFER_BIT;
			glClearDepthf(depth);
		}
		if( clear_flags & ClearFlags::Stencil )
		{
			clear_bits |= GL_STENCIL_BUFFER_BIT;
			glClearStencil(stencil);
		}
		if( clear_bits )
		{
			glClear(clear_bits);
		}
	}

	void RenderDeviceGLES2::present()
	{
#ifdef WIN32
		eglSwapBuffers( m_private->egl_display, m_private->egl_surface );
#endif //WIN32
	}

	VertexFormatHandle RenderDeviceGLES2::create_vertex_format( const VertexFormatDescr& fmt )
	{
		VertexFormatGLES2 res;
		res.descr = fmt;
		return m_resources->vertex_formats.push(res);
	}

	VertexShaderHandle RenderDeviceGLES2::create_vertex_shader( const ShaderCode& code )
	{
		VertexShaderGLES2 res;
		res.native = gles2_compile_shader(code.data(), ShaderType::Vertex);
		if( res.native )
		{
			return m_resources->vertex_shaders.push(res);
		}
		else
		{
			return -1;
		}
	}

	PixelShaderHandle RenderDeviceGLES2::create_pixel_shader( const ShaderCode& code )
	{
		PixelShaderGLES2 res;
		res.native = gles2_compile_shader(code.data(), ShaderType::Pixel);
		if( res.native )
		{
			return m_resources->pixel_shaders.push(res);
		}
		else
		{
			return -1;
		}
	}

	TextureHandle RenderDeviceGLES2::create_texture( const TextureDescr& tex, const void* data, size_t size )
	{
		TextureGLES2 res;

		res.descr = tex;

		glPixelStorei ( GL_UNPACK_ALIGNMENT, 1 );
		glGenTextures(1, &res.native);
		glBindTexture(GL_TEXTURE_2D, res.native);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tex.width, tex.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
		glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

		return m_resources->textures.push(res);
		
	}

	TextureHandle RenderDeviceGLES2::load_texture( const char* filename, TextureType::type type )
	{
		if( type != TextureType::Tex2D )
		{
			Log::error("Only loading simple 2D textures is currently supported.");
			return -1;
		}

		Image img;
		if( img.load_dds(filename) )
		{
			TextureGLES2 res;

			res.descr = TextureDescr::create_2D(img.width(), img.height(), img.format(), false, false);

			glPixelStorei ( GL_UNPACK_ALIGNMENT, 1 );
			glGenTextures(1, &res.native);
			glBindTexture(GL_TEXTURE_2D, res.native);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img.width(), img.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, img.data());
			glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
			glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

			return m_resources->textures.push(res);
		}

		return -1;
	}

	TextureHandle RenderDeviceGLES2::create_render_target( const TextureDescr& /*tex*/ )
	{
		return -1;
	}

	IndexBufferHandle RenderDeviceGLES2::create_index_buffer( const IndexBufferDescr& ib, const void* data )
	{
		R_ASSERT_MSG((ib.mode == BufferMode::WriteOnce && data==NULL)==false,  "Static buffers must provide valid initial data.");
		
		IndexBufferGLES2 res;

		res.descr = ib;
		
		glGenBuffers(1, &res.native);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, res.native);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, ib.size, data, gles2_convert(ib.mode));

		return m_resources->index_buffers.push(res);
	}

	VertexBufferHandle RenderDeviceGLES2::create_vertex_buffer( const VertexBufferDescr& vb, const void* data )
	{
		R_ASSERT_MSG((vb.mode == BufferMode::WriteOnce && data==NULL)==false,  "Static buffers must provide valid initial data.");

		VertexBufferGLES2 res;

		res.descr = vb;

		glGenBuffers(1, &res.native);
		glBindBuffer(GL_ARRAY_BUFFER, res.native);
		glBufferData(GL_ARRAY_BUFFER, vb.size, data, gles2_convert(vb.mode));

		return m_resources->vertex_buffers.push(res);
	}



	VertexBufferLock RenderDeviceGLES2::lock_vertex_buffer( VertexBufferHandle /*h*/, uint32 /*offset*/, uint32 /*size*/ )
	{

	}

	void RenderDeviceGLES2::unlock_index_buffer( IndexBufferLock& /*lock*/ )
	{

	}

	void RenderDeviceGLES2::unlock_vertex_buffer( VertexBufferLock& /*lock*/ )
	{

	}	


	void RenderDeviceGLES2::destroy_index_buffer( IndexBufferHandle /*h*/ )
	{

	}

	void RenderDeviceGLES2::destroy_vertex_buffer( VertexBufferHandle /*h*/ )
	{

	}

	void RenderDeviceGLES2::destroy_vertex_shader( VertexShaderHandle& /*h*/ )
	{

	}

	void RenderDeviceGLES2::destroy_pixel_shader( PixelShaderHandle& /*h*/ )
	{

	}

	void RenderDeviceGLES2::destroy_texture( TextureHandle& /*h*/ )
	{

	}

	const TextureDescr& RenderDeviceGLES2::texture_descr( TextureHandle h ) const
	{
		if( h.valid() )
		{
			return m_resources->textures[h.index()].descr;
		}
		else
		{
			static const TextureDescr invalid_texture = TextureDescr::create_2D(1,1,PixelFormat::Unknown,false,false);
			return invalid_texture;
		}
	}

	BlendStateHandle RenderDeviceGLES2::create_blend_state( const BlendStateDescr& descr )
	{
		BlendStateGLES2 res;
		res.descr = descr;
		return m_resources->blend_states.push(res);
	}

	SamplerStateHandle RenderDeviceGLES2::create_sampler_state( const SamplerStateDescr& descr )
	{
		SamplerStateGLES2 res;
		res.descr = descr;
		return m_resources->sampler_states.push(res);
	}

	void RenderDeviceGLES2::destroy_blend_state( BlendStateHandle /*h*/ )
	{
		// nothing to do
	}

	void RenderDeviceGLES2::destroy_sampler_state( SamplerStateHandle /*h*/ )
	{
		// nothing to do
	}

	void RenderDeviceGLES2::destroy_technique( TechniqueHandle /*h*/ )
	{
		//TODO
	}



	TechniqueHandle RenderDeviceGLES2::create_technique( const TechniqueDescr& descr )
	{
		GLint ps = m_resources->pixel_shaders[descr.ps.index()].native;
		GLint vs = m_resources->vertex_shaders[descr.vs.index()].native;

		const VertexFormatDescr& vf = m_resources->vertex_formats[descr.vs.index()].descr;

		GLuint programObject = glCreateProgram();
		if ( programObject == 0 ) return -1;

		glAttachShader ( programObject, ps );
		glAttachShader ( programObject, vs );

		// Link the program
		glLinkProgram ( programObject );

		// Check the link status
		GLint linked;
		glGetProgramiv ( programObject, GL_LINK_STATUS, &linked );

		if ( !linked ) 
		{
			GLint infoLen = 0;

			glGetProgramiv ( programObject, GL_INFO_LOG_LENGTH, &infoLen );

			if ( infoLen > 1 )
			{
				char* infoLog = (char*)malloc (sizeof(char) * infoLen );

				glGetProgramInfoLog ( programObject, infoLen, NULL, infoLog );
				Log::error( "Error linking program:\n%s\n", infoLog );

				free ( infoLog );
			}

			glDeleteProgram ( programObject );
			return -1;
		}

		TechniqueGLES2 res;

		res.native = programObject;
		res.vertex_array_object = 0;

		const ShaderConstantBindings* bindings = descr.bindings;
		if( bindings )
		{
			for( uint32 i=0; i<bindings->items.size(); ++i )
			{
				const ShaderConstantBindings::Item& item = bindings->items[i];
				GLint location = glGetUniformLocation(programObject, item.name);
				if( location!=-1 )
				{
					if( item.type == ShaderConstantBindings::BindingType_Constant )
					{
						TechniqueGLES2::UniformBinding b;
						b.index = location;
						b.data  = item.data;
						b.count = item.count;
						res.uniforms.push_back(b);
					}
					else if( item.type == ShaderConstantBindings::BindingType_Sampler )
					{
						TechniqueGLES2::SamplerBinding b;
						b.index = item.idx;
						b.location = location;
						res.samplers.push_back(b);
					}
				}
			}
		}
		
		for( uint32 i=0; i<vf.num_elements(); ++i )
		{
			const VertexFormatDescr::Element& elem = vf.element(i);
			char elem_name[256];
			sprintf(elem_name, "%s%d", usage_type_to_string(elem.usage), elem.index);
			GLint location = glGetAttribLocation(programObject, elem_name);
			if( location!=-1 )
			{
				TechniqueGLES2::AttributeBinding b;
				b.stream = elem.stream;
				b.location = location;
				b.type = vertex_data_type_to_gles2(elem.type);
				b.count = vertex_data_type_to_count(elem.type);
				b.normalized = vertex_data_type_to_normalized(elem.type);
				b.stride = vf.stream_stride(elem.stream);
				b.offset = elem.offset;
				res.attributes.push_back(b);
			}
		}

		return m_resources->techniques.push(res);
	}

	void RenderDeviceGLES2::update_index_buffer( IndexBufferHandle h, void* data, uint32 offset, uint32 size, bool discard )
	{
		IndexBufferGLES2& ib = m_resources->index_buffers[h.index()];
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ib.native);
		
        if( offset==0 && ((size==0||size==ib.descr.size)||discard) )
		{
            if( size==0 ) size = ib.descr.size;
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, gles2_convert(ib.descr.mode));
		}
		else
		{
			glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, offset, size, data);
		}
	}

	void RenderDeviceGLES2::update_vertex_buffer( VertexBufferHandle h, void* data, uint32 offset, uint32 size, bool discard )
	{
		VertexBufferGLES2& vb = m_resources->vertex_buffers[h.index()];
		glBindBuffer(GL_ARRAY_BUFFER, vb.native);
        
		if( offset==0 && ((size==0||size==vb.descr.size) || discard) )
		{
            if( size==0 ) size = vb.descr.size;
			glBufferData(GL_ARRAY_BUFFER, size, data, gles2_convert(vb.descr.mode));
		}
		else
		{
			glBufferSubData(GL_ARRAY_BUFFER, offset, size, data);
		}
	}

	//////////////////////////////////////////////////////////////////////////


	RenderContextGLES2::RenderContextGLES2(){}

	void RenderContextGLES2::draw( uint32 first_vertex, uint32 vertex_count )
	{
        m_stats->draw_calls += 1;
        m_stats->vertices   += vertex_count;
        
		glDrawArrays(m_primitive_type, first_vertex, vertex_count);
	}

	void RenderContextGLES2::draw_indexed( uint32 index_count, uint32 first_index, uint32 /*base_vertex*/, uint32 /*num_vertices*/ )
	{
        m_stats->draw_calls += 1;
        m_stats->vertices   += index_count;
        
		glDrawElements(m_primitive_type, index_count, m_index_type, gl_buffer_offset(first_index*m_index_size));
	}

	void RenderContextGLES2::init( RenderDeviceStats* stats, RenderResourcesGLES2* resources )
	{
        m_stats = stats;
		m_enabled_vertex_attributes.reserve(128);
		m_resources = resources;
		m_primitive_type = GL_TRIANGLES;
		primitive_type.set_next(PrimitiveTypes::TriangleList);
		m_index_size = 0;
		m_index_type = 0;
	}

	void RenderContextGLES2::commit()
	{
		if( technique.is_dirty() || vertex_streams.is_dirty() || ps_textures.is_dirty() )
		{
			const TechniqueGLES2& t = m_resources->techniques[technique.next().index()];

            if( technique.is_dirty() )
            {            
                glUseProgram(t.native);
            }

			if( technique.is_dirty() || vertex_streams.is_dirty() )
			{                    
				for( uint32 i=0; i<m_enabled_vertex_attributes.size(); ++i )
				{
					glDisableVertexAttribArray(m_enabled_vertex_attributes[i]);
				}
				m_enabled_vertex_attributes.clear();

				for( uint32 i=0; i<t.attributes.size(); ++i )
				{
					const TechniqueGLES2::AttributeBinding& attr = t.attributes[i];
					if( vertex_streams.next(attr.stream).valid() )
					{
						const VertexBufferGLES2& vb = m_resources->vertex_buffers[vertex_streams.next(attr.stream).index()];
						glBindBuffer(GL_ARRAY_BUFFER, vb.native);
						glVertexAttribPointer(attr.location, attr.count, attr.type, attr.normalized, attr.stride, gl_buffer_offset(attr.offset));
						glEnableVertexAttribArray(attr.location);
						m_enabled_vertex_attributes.push_back(attr.location);
					}
                }
                
                vertex_streams.commit();
                technique.commit();
            }

			for(uint32 i=0; i<t.uniforms.size(); ++i)
			{
				const TechniqueGLES2::UniformBinding& b = t.uniforms[i];
				glUniform4fv(b.index, b.count, (const GLfloat*)b.data);
			}

            if( ps_textures.is_dirty() )
            {
                for(uint32 i=0; i<t.samplers.size(); ++i)
                {
                    const TechniqueGLES2::SamplerBinding& b = t.samplers[i];
                    TextureHandle texture_handle = ps_textures.next(b.index);
                    const TextureGLES2& texture = m_resources->textures[texture_handle.index()];
                    glActiveTexture(GL_TEXTURE0 + b.index);
                    glBindTexture(GL_TEXTURE_2D, texture.native);
                    glUniform1i(b.location, b.index);
                }
                
                ps_textures.commit();
            }			
		}

		if( blend_state.is_dirty() )
		{
			const BlendStateGLES2& state = m_resources->blend_states[blend_state.next().index()];
			const BlendStateDescr& descr = state.descr;
			
			if( descr.enable ) 
			{
				glEnable(GL_BLEND);
				glBlendFunc(gles2_convert(descr.src), gles2_convert(descr.dst));
				glBlendEquation(gles2_convert(descr.op));
			}
			else 
			{
				glDisable(GL_BLEND);
			}

			blend_state.commit();
		}

		if( index_stream.is_dirty() )
		{
			const IndexBufferGLES2 ib = m_resources->index_buffers[index_stream.next().index()];
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ib.native);
			m_index_type = (ib.descr.type==IndexFormat::Index16) ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT;
			m_index_size = (ib.descr.type==IndexFormat::Index16) ? 2 : 4;
			index_stream.commit();
		}

		if( primitive_type.is_dirty() )
		{
			PrimitiveType pt = primitive_type.next();

			switch(pt)
			{
			case PrimitiveTypes::PointList:
				m_primitive_type = GL_POINTS;
				break;
			case PrimitiveTypes::LineList:
				m_primitive_type = GL_LINES;
				break;
			case PrimitiveTypes::LineStrip:
				m_primitive_type = GL_LINE_STRIP;
				break;
			case PrimitiveTypes::TriangleList:
				m_primitive_type = GL_TRIANGLES;
				break;
			case PrimitiveTypes::TriangleStrip:
				m_primitive_type = GL_TRIANGLE_STRIP;
				break;
			default:
				m_primitive_type = 0;
				break;
			}
            
			primitive_type.commit();
		}

	}

	void RenderDeviceGLES2::reset_stats()
	{
		rush_memset(&m_stats, 0, sizeof(m_stats));
	}

	const RenderDeviceStats& RenderDeviceGLES2::stats()
	{
		return m_stats;
	}
#endif //0


    // various
    RenderDevice* Gfx_CreateDevice(Window* window, const RenderDeviceConfig& cfg)
    {
        return NULL;
    }

    void Gfx_DestroyDevice(RenderDevice* dev)
    {

    }

    void Gfx_AddReference(RenderDevice* dev)
    {

    }

    void Gfx_RemoveReference(RenderDevice* dev)
    {

    }

    void Gfx_FrameBegin(RenderDevice* dev)
    {

    }

    void Gfx_FrameEnd(RenderDevice* dev)
    {

    }

    void Gfx_Clear(RenderContext* rc, ColourRGBA8 colour, ClearFlags::type clear_flags, float depth, uint32 stencil)
    {

    }

    void Gfx_Present(RenderDevice* dev)
    {

    }

    const RenderDeviceStats& Gfx_Stats(RenderDevice* dev)
    {
        static const RenderDeviceStats stats = {0,0};
        return stats;
    }

    void Gfx_ResetStats(RenderDevice* dev)
    {

    }

    // vertex format
    VertexFormatHandle Gfx_VertexFormat_Create(RenderDevice* dev, const VertexFormatDescr& fmt)
    {
        return InvalidResourceHandle();
    }

    void Gfx_VertexFormat_Destroy(RenderDevice* dev, VertexFormatHandle h)
    {

    }

    // vertex shader
    VertexShaderHandle Gfx_VertexShader_Create(RenderDevice* dev, const ShaderCode& code)
    {
        return InvalidResourceHandle();
    }

    void Gfx_VertexShader_Destroy(RenderDevice* dev, VertexShaderHandle& vsh)
    {

    }

    // pixel shader
    PixelShaderHandle Gfx_PixelShader_Create(RenderDevice* dev, const ShaderCode& code)
    {
        return InvalidResourceHandle();
    }

    void Gfx_PixelShader_Destroy(RenderDevice* dev, PixelShaderHandle& psh)
    {

    }

    // technique
    TechniqueHandle Gfx_Technique_Create(RenderDevice* dev, const TechniqueDescr& descr)
    {
        return InvalidResourceHandle();
    }

    void Gfx_Technique_Destroy(RenderDevice* dev, TechniqueHandle h)
    {

    }

    // texture
    TextureHandle Gfx_Texture_Load(RenderDevice* dev, const char* filename, TextureType::type type)
    {
        return InvalidResourceHandle();
    }

    TextureHandle Gfx_Texture_Create(RenderDevice* dev, const TextureDescr& tex, const void* data, size_t size)
    {
        return InvalidResourceHandle();
    }

    const TextureDescr& Gfx_Texture_Descr(RenderDevice* dev, TextureHandle h)
    {
        static const TextureDescr invalid_texture = TextureDescr::create_2D(1,1,PixelFormat::Unknown,false,false);
        return invalid_texture;
    }

    void Gfx_Texture_Destroy(RenderDevice* dev, TextureHandle& th)
    {

    }

    // blend state
    BlendStateHandle Gfx_BlendState_Create(RenderDevice* dev, const BlendStateDescr& descr)
    {
        return InvalidResourceHandle();
    }

    void Gfx_BlendState_Destroy(RenderDevice* dev, BlendStateHandle h)
    {

    }

    // sampler state
    SamplerStateHandle Gfx_SamplerState_Create(RenderDevice* dev, const SamplerStateDescr& descr)
    {
        return InvalidResourceHandle();
    }

    void Gfx_SamplerState_Destroy(RenderDevice* dev, SamplerStateHandle h)
    {

    }

    // render target
    TextureHandle Gfx_RenderTarget_Create(RenderDevice* dev, const TextureDescr& tex)
    {
        return InvalidResourceHandle();
    }

    // vertex buffer
    VertexBufferHandle Gfx_VertexBuffer_Create(RenderDevice* dev, const VertexBufferDescr& vb, const void* data)
    {
        return InvalidResourceHandle();
    }

    VertexBufferLock Gfx_VertexBuffer_Lock(RenderDevice* dev, VertexBufferHandle vb, uint32 offset, uint32 size)
    {
        Log::error("Locking buffers is not supported on this platform!");
        VertexBufferLock lock;
        return lock;
    }

    void Gfx_VertexBuffer_Unlock(RenderDevice* dev, VertexBufferLock& lock)
    {

    }

    void Gfx_VertexBuffer_Update(RenderDevice* dev, VertexBufferHandle h, void* data, uint32 offset, uint32 size, bool discard)
    {

    }

    void Gfx_VertexBuffer_Destroy(RenderDevice* dev, VertexBufferHandle vb)
    {

    }

    // index buffer
    IndexBufferHandle Gfx_IndexBuffer_Create(RenderDevice* dev, const IndexBufferDescr& ib, const void* data)
    {
        return InvalidResourceHandle();
    }

    IndexBufferLock Gfx_IndexBuffer_Lock(RenderDevice* dev, IndexBufferHandle ib, uint32 offset, uint32 size)
    {
        Log::error("Locking buffers is not supported on this platform!");
        IndexBufferLock lock;
        return lock;
    }

    void Gfx_IndexBuffer_Unlock(RenderDevice* dev, IndexBufferLock& lock)
    {

    }

    void Gfx_IndexBuffer_Update(RenderDevice* dev, IndexBufferHandle h, void* data, uint32 offset, uint32 size, bool discard)
    {

    }

    void Gfx_IndexBuffer_Destroy(RenderDevice* dev, IndexBufferHandle ib)
    {

    }

    // context
    RenderContext* Gfx_Immediate_Context(RenderDevice* dev)
    {
        return NULL;
    }

    void Gfx_Context_SetViewport(RenderContext* rc, const Viewport& _viewport)
    {

    }

    void Gfx_Context_SetTechnique(RenderContext* rc, TechniqueHandle h)
    {

    }

    void Gfx_Context_SetPrimitive(RenderContext* rc, PrimitiveType type)
    {

    }

    void Gfx_Context_SetIndexStream(RenderContext* rc, IndexBufferHandle h)
    {

    }

    void Gfx_Context_SetVertexStream(RenderContext* rc, uint32 idx, VertexBufferHandle h)
    {

    }

    void Gfx_Context_SetPixelTexture(RenderContext* rc, uint32 idx, TextureHandle h)
    {

    }

    void Gfx_Context_SetPixelSamplerState(RenderContext* rc, uint32 idx, SamplerStateHandle next_state)
    {

    }

    void Gfx_Context_SetBlendState(RenderContext* rc, BlendStateHandle next_state)
    {

    }

    void Gfx_Context_Draw(RenderContext* rc, uint32 first_vertex, uint32 vertex_count)
    {

    }

    void Gfx_Context_DrawIndexed(RenderContext* rc, uint32 index_count, uint32 first_index, uint32 base_vertex, uint32 num_vertices)
    {

    }

}

#else //RUSH_RENDER_API==RUSH_RENDER_API_GLES2
char _GLES2RenderDevice_cpp; // suppress linker warning
#endif //RUSH_RENDER_API==RUSH_RENDER_API_GLES2

