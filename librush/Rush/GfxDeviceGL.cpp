#include "GfxDeviceGL.h"

#if RUSH_RENDER_API==RUSH_RENDER_API_GL

#include "GfxDevice.h"
#include "GfxBuffer.h"
#include "GfxVertexFormat.h"
#include "GfxShader.h"
#include "Window.h"
#include "UtilLog.h"
#include "UtilIndexedPool.h"
#include "UtilImage.h"
#include "UtilReferenceCount.h"

#include <stdio.h> // for sprintf

#if defined (RUSH_PLATFORM_MACOS)
#include <OpenGL/gl.h>
#endif

#if defined (RUSH_PLATFORM_WINDOWS)
#include <windows.h>
#include <GL/gl.h>
// OpenGL Extensions
typedef char GLchar;
typedef ptrdiff_t GLintptr;
typedef ptrdiff_t GLsizeiptr;
#define GL_FUNC_ADD									0x8006
#define GL_MIN_EXT									0x8007
#define GL_MAX_EXT									0x8008
#define GL_FUNC_SUBTRACT							0x800A
#define GL_FUNC_REVERSE_SUBTRACT					0x800B
#define GL_STREAM_DRAW								0x88E0
#define GL_STATIC_DRAW								0x88E4
#define GL_DYNAMIC_DRAW								0x88E8
#define GL_FRAGMENT_SHADER							0x8B30
#define GL_VERTEX_SHADER							0x8B31
#define GL_TEXTURE0									0x84C0
#define GL_ELEMENT_ARRAY_BUFFER						0x8893
#define GL_INFO_LOG_LENGTH							0x8B84
#define GL_LINK_STATUS								0x8B82
#define GL_COMPILE_STATUS							0x8B81
#define GL_ARRAY_BUFFER								0x8892
GLvoid	(APIENTRY * glBindBuffer)					(GLenum target, GLuint buffer) = NULL;
GLuint	(APIENTRY * glCreateShader)					(GLenum type) = NULL;
void	(APIENTRY * glShaderSource)					(GLuint shader, GLsizei count, const GLchar** strings, const GLint* lengths) = NULL;
void	(APIENTRY * glAttachShader)					(GLuint program, GLuint shader) = NULL;
void	(APIENTRY * glCompileShader)				(GLuint shader) = NULL;
void	(APIENTRY * glGetProgramiv)					(GLuint id, GLenum pname, GLint* params) = NULL;
void	(APIENTRY * glGenBuffers)					(GLsizei n, GLuint* buffers) = NULL;
void	(APIENTRY * glBufferData)					(GLenum target, GLsizeiptr size, const GLvoid* data, GLenum usage) = NULL;
void	(APIENTRY * glGetShaderiv)					(GLuint shader, GLenum pname, GLint* param) = NULL;
void	(APIENTRY * glGetShaderInfoLog)				(GLuint shader, GLsizei bufSize, GLsizei* length, GLchar* infoLog) = NULL;
void	(APIENTRY * glDeleteShader)					(GLuint shader) = NULL;
void	(APIENTRY * glBufferSubData)				(GLenum target, GLintptr offset, GLsizeiptr size, const GLvoid* data) = NULL;
void	(APIENTRY * glUseProgram)					(GLuint program) = NULL;
void	(APIENTRY * glDisableVertexAttribArray)		(GLuint index) = NULL;
void	(APIENTRY * glEnableVertexAttribArray)		(GLuint index) = NULL;
void	(APIENTRY * glVertexAttribPointer)			(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid* pointer) = NULL;
void	(APIENTRY * glActiveTexture)				(GLenum texture) = NULL;
void	(APIENTRY * glUniform1i)					(GLint location, GLint v0) = NULL;
void	(APIENTRY * glUniformMatrix4fv)				(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value) = NULL;
void	(APIENTRY * glUniform4fv)					(GLint location, GLsizei count, const GLfloat* value) = NULL;
void	(APIENTRY * glBlendEquation)				(GLenum mode) = NULL;
GLuint	(APIENTRY * glCreateProgram)				(void) = NULL;
void	(APIENTRY * glLinkProgram)					(GLuint program) = NULL;
void	(APIENTRY * glGetProgramInfoLog)			(GLuint program, GLsizei bufSize, GLsizei* length, GLchar* infoLog) = NULL;
void	(APIENTRY * glDeleteProgram)				(GLuint program) = NULL;
GLint	(APIENTRY * glGetUniformLocation)			(GLuint program, const GLchar* name) = NULL;
GLint	(APIENTRY * glGetAttribLocation)			(GLuint program, const GLchar* name) = NULL;
BOOL	(APIENTRY * wglSwapIntervalEXT)				(int interval) = NULL;
void	(APIENTRY * glBlendEquationSeparate)		(GLenum eqRGB, GLenum eqAlpha) = NULL;
void	(APIENTRY * glBlendFuncSeparate)			(GLenum srcRGB, GLenum dstRGB, GLenum srcAlpha, GLenum dstAlpha) = NULL;
namespace Rush 
{
	template <typename T>
	inline void get_gl_proc(T& res, const char* name)
	{
		res = (T)wglGetProcAddress(name);
		if( res == NULL ){
			Log::error("Failed to get OpenGL function '%s', error code %u", name, GetLastError());
		}
	}
}
#endif //RUSH_PLATFORM_WINDOWS

namespace Rush 
{
	enum StateDirtyFlag
	{
		StateDirtyFlag_Technique        = 1<<0,
		StateDirtyFlag_VertexStream     = 1<<1,
		StateDirtyFlag_PixelTexture     = 1<<2
	};

	inline void* gl_buffer_offset(uint32 offset)
	{
		return ((char*)(NULL)) + offset;
	}

	inline GLenum gl_convert(BufferMode::type usage)
	{
		switch(usage)
		{
		case BufferMode::WriteOnce:		return GL_STATIC_DRAW;
		case BufferMode::WriteOften:	return GL_STREAM_DRAW;
		case BufferMode::WriteRarely:	return GL_DYNAMIC_DRAW;		
		default:						return GL_DYNAMIC_DRAW;
		}
	}

	static inline GLenum gl_convert(BlendOp::type op)
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

	static inline GLenum gl_convert(Blend::type blend)
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

	static inline GLint vertex_data_type_to_count( VertexFormatDescr::DataType::type type )
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

	static inline GLenum vertex_data_type_to_gl( VertexFormatDescr::DataType::type type )
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

	static inline GLboolean vertex_data_type_to_normalized( VertexFormatDescr::DataType::type type )
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

	static GLuint gl_compile_shader(const char* source, ShaderType::type type)
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

	struct VertexFormatGL
	{
		VertexFormatDescr descr;
	};

	struct VertexShaderGL
	{
		GLuint native;
	};

	struct PixelShaderGL
	{
		GLuint native;
	};

	struct TextureGL
	{
		TextureDescr descr;
		GLuint native;
	};

	struct IndexBufferGL
	{
		IndexBufferDescr descr;
		GLuint native;
	};

	struct VertexBufferGL
	{
		VertexBufferDescr descr;
		GLuint native;
	};

	struct SamplerStateGL
	{
		SamplerStateDescr descr;
	};

	struct BlendStateGL
	{
		BlendStateDescr descr;
	};

	struct TechniqueGL
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
			GLint location;
			GLint count;
			GLenum type;
			GLsizei stride;
			uint16 offset;
			uint16 stream;
			GLboolean normalized;            
		};

		GLuint native;
		GLuint vertex_array_object;

		DynamicArray<UniformBinding>    uniforms;
		DynamicArray<AttributeBinding>  attributes;
		DynamicArray<SamplerBinding>    samplers;
	};

	struct RenderResourcesGL
	{
		ResourcePool<VertexFormatGL,	VertexFormatHandle, 	GfxMaxVertexFormats>	vertex_formats;
		ResourcePool<VertexShaderGL,	VertexShaderHandle, 	GfxMaxVertexShaders>	vertex_shaders;
		ResourcePool<PixelShaderGL,		PixelShaderHandle, 		GfxMaxPixelShaders>		pixel_shaders;
		ResourcePool<TextureGL,			TextureHandle, 			GfxMaxTextures>			textures;
		ResourcePool<IndexBufferGL,		IndexBufferHandle, 		GfxMaxIndexBuffers>		index_buffers;
		ResourcePool<VertexBufferGL,	VertexBufferHandle, 	GfxMaxVertexBuffers>	vertex_buffers;
		ResourcePool<SamplerStateGL,	SamplerStateHandle, 	GfxMaxSamplerStates>	sampler_states;
		ResourcePool<BlendStateGL,		BlendStateHandle, 		GfxMaxBlendStates>		blend_states;
		ResourcePool<TechniqueGL,		TechniqueHandle, 		GfxMaxTechniques>		techniques;
	};

	class RenderContext
	{
	public:
		enum
		{
			MaxTextures      = 16,
			MaxVertexStreams = 8
		};

		RenderContext()
			: primitive_type(GL_TRIANGLES)
			, index_type(0)
			, index_size(0)
			, resources(NULL)
			, dirty_state_flags(0)
			, dirty_vertex_streams(0)
		{
			enabled_vertex_attributes.reserve(128);
		}

		GLint 	primitive_type;
		GLint 	index_type;
		int 	index_size;
		RenderResourcesGL* resources;

		uint32              dirty_state_flags;
		TechniqueHandle     pending_technique;
		TextureHandle       pending_pixel_textures[MaxTextures];
		VertexBufferHandle  pending_vertex_streams[MaxVertexStreams];
		uint32              dirty_vertex_streams;

		DynamicArray<uint32> enabled_vertex_attributes;
	};

	class RenderDevice
	{
	public:
		ReferenceCount refct;
		RenderResourcesGL resources;
		RenderContext context;
		RenderDeviceConfig config;
		Window* window;

#if defined(RUSH_PLATFORM_WINDOWS)
		HGLRC glcontext;
		HDC hdc;
		HWND hwnd;
#endif //RUSH_PLATFORM_WINDOWS
	};

	void Gfx_AddReference(RenderDevice* dev)
	{
		dev->refct.add_reference();
	}

	void Gfx_RemoveReference(RenderDevice* dev)
	{
		dev->refct.remove_reference();
	}

	void Gfx_Clear(RenderContext* /*rc*/, ColourRGBA8 colour, ClearFlags::type clear_flags, float depth, uint32 stencil)
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
			glClearDepth(depth);
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

	const TextureDescr& Gfx_Texture_Descr(RenderDevice* dev, TextureHandle h)
	{
		if( h.valid() )
		{
			return dev->resources.textures[h].descr;
		}
		else
		{
			static const TextureDescr invalid_texture = TextureDescr::create_2D(1,1,PixelFormat::Unknown,false,false);
			return invalid_texture;
		}
	}

	inline void Gfx_Commit_GL(RenderContext* rc)
	{
		const uint32 dirty = rc->dirty_state_flags;

		if( (dirty&StateDirtyFlag_Technique) || (dirty&StateDirtyFlag_VertexStream) || (dirty&StateDirtyFlag_PixelTexture) )
		{
			const TechniqueGL& t = rc->resources->techniques[rc->pending_technique];

			if( dirty&StateDirtyFlag_Technique )
			{
				glUseProgram(t.native);
			}

			if( (dirty&StateDirtyFlag_Technique) || (dirty&StateDirtyFlag_VertexStream) )
			{
				for( uint32 i=0; i<rc->enabled_vertex_attributes.size(); ++i )
				{
					glDisableVertexAttribArray(rc->enabled_vertex_attributes[i]);
				}
				rc->enabled_vertex_attributes.clear();

				for( uint32 i=0; i<t.attributes.size(); ++i )
				{
					const TechniqueGL::AttributeBinding& attr = t.attributes[i];
					const VertexBufferHandle vbh = rc->pending_vertex_streams[attr.stream];
					if( vbh.valid() )
					{
						const VertexBufferGL& vb = rc->resources->vertex_buffers[vbh];
						glBindBuffer(GL_ARRAY_BUFFER, vb.native);
						glVertexAttribPointer(attr.location, attr.count, attr.type, attr.normalized, attr.stride, gl_buffer_offset(attr.offset));
						glEnableVertexAttribArray(attr.location);
						rc->enabled_vertex_attributes.push_back(attr.location);
					}
				}                
			}

			// TODO: Need to unset previously set but now unused textures
			if( dirty&StateDirtyFlag_PixelTexture )
			{
				for(uint32 i=0; i<t.samplers.size(); ++i)
				{
					const TechniqueGL::SamplerBinding& b = t.samplers[i];
					TextureHandle texture_handle = rc->pending_pixel_textures[b.index];
					const TextureGL& texture = rc->resources->textures[texture_handle];
					glActiveTexture(GL_TEXTURE0 + b.index);
					glBindTexture(GL_TEXTURE_2D, texture.native);
					glUniform1i(b.location, b.index);
				}
			}
		}

		// TODO: This is temporary here, until constant buffers are added
		if( rc->pending_technique.valid() )
		{
			const TechniqueGL& t = rc->resources->techniques[rc->pending_technique];
			for(uint32 i=0; i<t.uniforms.size(); ++i)
			{
				const TechniqueGL::UniformBinding& b = t.uniforms[i];
				if(b.count == 4 ) // TODO: fix this dirty hack (right now assuming that 4 vec4s are always a matrix)
				{
					glUniformMatrix4fv(b.index, 1, false, (const GLfloat*)b.data);
				}
				else
				{
					glUniform4fv(b.index, b.count, (const GLfloat*)b.data);
				}
			}
		}

		rc->dirty_state_flags = 0;
	}

	void Gfx_Context_Draw(RenderContext* rc, uint32 first_vertex, uint32 vertex_count)
	{
		Gfx_Commit_GL(rc);
		glDrawArrays(rc->primitive_type, first_vertex, vertex_count);
	}

	void Gfx_Context_DrawIndexed(RenderContext* rc, uint32 index_count, uint32 first_index, uint32 base_vertex, uint32 num_vertices)
	{
		R_UNUSED(num_vertices);
		R_UNUSED(base_vertex);

		Gfx_Commit_GL(rc);
		glDrawElements(rc->primitive_type, index_count, rc->index_type, gl_buffer_offset(first_index*rc->index_size));
	}

	void Gfx_FrameBegin(RenderDevice*)
	{
		// nothing
	}

	void Gfx_FrameEnd(RenderDevice*)
	{
		// nothing
	}

	void Gfx_Present(RenderDevice* dev)
	{
#ifdef RUSH_PLATFORM_WINDOWS
		wglSwapIntervalEXT(dev->config.use_vertical_sync ? 1 : 0);
		SwapBuffers(dev->hdc);
#else //RUSH_PLATFORM_WINDOWS
		(void)dev; // TODO
#endif //RUSH_PLATFORM_WINDOWS
	}

	const RenderDeviceStats& Gfx_Stats(RenderDevice* dev)
	{
		// TODO
		(void)dev;
		static const RenderDeviceStats dummy_stats = {0,0};
		return dummy_stats;
	}

	void Gfx_ResetStats(RenderDevice* dev)
	{
		// TODO
		(void)dev; 
	}

	void Gfx_Context_SetPixelSamplerState(RenderContext* rc, uint32 idx, SamplerStateHandle h)
	{
		R_UNUSED(h);
		R_UNUSED(idx);
		R_UNUSED(rc);
		// TODO
	}

	void Gfx_Context_SetDepthStencilState(RenderContext* rc, DepthStencilStateHandle next_state)
	{
		R_UNUSED(rc);
		R_UNUSED(next_state);
		// TODO
	}

	void Gfx_DepthStencilState_Destroy(RenderDevice* dev, DepthStencilStateHandle h)
	{
		R_UNUSED(dev);
		R_UNUSED(h);
		// TODO
	}

	void Gfx_Context_SetVertexStream(RenderContext* rc, uint32 idx, VertexBufferHandle h)
	{
		R_ASSERT(idx<RenderContext::MaxVertexStreams);
		rc->pending_vertex_streams[idx] = h;
		rc->dirty_vertex_streams |= 1<<idx;
		rc->dirty_state_flags |= StateDirtyFlag_VertexStream;
	}	

	void Gfx_Context_SetPixelTexture(RenderContext* rc, uint32 idx, TextureHandle h)
	{
		R_ASSERT(idx<RenderContext::MaxTextures);
		rc->pending_pixel_textures[idx] = h;
		rc->dirty_state_flags |= StateDirtyFlag_PixelTexture;
	}

	void Gfx_Context_SetIndexStream(RenderContext* rc, IndexBufferHandle h)
	{
		const IndexBufferGL ib = rc->resources->index_buffers[h];
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ib.native);
		rc->index_type = (ib.descr.type==IndexFormat::Index16) ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT;
		rc->index_size = (ib.descr.type==IndexFormat::Index16) ? 2 : 4;
	}

	void Gfx_Context_SetBlendState(RenderContext* rc, BlendStateHandle h)
	{
		const BlendStateGL& state = rc->resources->blend_states[h];
		const BlendStateDescr& descr = state.descr;

		//TODO: only colour blend states are implemented, still need to support separate alpha states

		if( descr.enable )
		{
			glEnable(GL_BLEND);
			glBlendFuncSeparate(gl_convert(descr.src), gl_convert(descr.dst), gl_convert(descr.alpha_src), gl_convert(descr.alpha_dst));
			glBlendEquationSeparate(gl_convert(descr.op), gl_convert(descr.alpha_op));
		}
		else 
		{
			glDisable(GL_BLEND);
		}
	}

	void Gfx_VertexShader_Destroy(RenderDevice* dev, VertexShaderHandle& h)
	{
		R_UNUSED(h);
		R_UNUSED(dev);
		// TODO
	};

	void Gfx_VertexFormat_Destroy(RenderDevice* dev, VertexFormatHandle h)
	{
		R_UNUSED(h);
		R_UNUSED(dev);
		// TODO
	}

	void Gfx_VertexBuffer_Destroy(RenderDevice* dev, VertexBufferHandle h)
	{
		R_UNUSED(h);
		R_UNUSED(dev);
		// TODO
	}

	void Gfx_SamplerState_Destroy(RenderDevice* dev, SamplerStateHandle h)
	{
		R_UNUSED(h);
		R_UNUSED(dev);
		// TODO
	}

	void Gfx_Context_SetTechnique(RenderContext* rc, TechniqueHandle h)
	{
		rc->pending_technique = h;
		rc->dirty_state_flags |= StateDirtyFlag_Technique;
	}

	void Gfx_Context_SetPrimitive(RenderContext* rc, PrimitiveType type)
	{
		switch(type)
		{
		case PrimitiveTypes::PointList:
			rc->primitive_type = GL_POINTS;
			break;
		case PrimitiveTypes::LineList:
			rc->primitive_type = GL_LINES;
			break;
		case PrimitiveTypes::LineStrip:
			rc->primitive_type = GL_LINE_STRIP;
			break;
		case PrimitiveTypes::TriangleList:
			rc->primitive_type = GL_TRIANGLES;
			break;
		case PrimitiveTypes::TriangleStrip:
			rc->primitive_type = GL_TRIANGLE_STRIP;
			break;
		default:
			rc->primitive_type = 0;
			break;
		}
	}

	void Gfx_IndexBuffer_Update(RenderDevice* dev, IndexBufferHandle h, void* data, uint32 offset, uint32 size, bool discard)
	{
		IndexBufferGL& ib = dev->resources.index_buffers[h];
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ib.native);

		if( offset==0 && ((size==0||size==ib.descr.size)||discard) )
		{
			if( size==0 ) size = ib.descr.size;
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, gl_convert(ib.descr.mode));
		}
		else
		{
			glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, offset, size, data);
		}
	}

	void Gfx_VertexBuffer_Update(RenderDevice* dev, VertexBufferHandle h, void* data, uint32 offset, uint32 size, bool discard)
	{
		VertexBufferGL& vb = dev->resources.vertex_buffers[h];
		glBindBuffer(GL_ARRAY_BUFFER, vb.native);

		if( offset==0 && ((size==0||size==vb.descr.size) || discard) )
		{
			if( size==0 ) size = vb.descr.size;
			glBufferData(GL_ARRAY_BUFFER, size, data, gl_convert(vb.descr.mode));
		}
		else
		{
			glBufferSubData(GL_ARRAY_BUFFER, offset, size, data);
		}
	}

	RenderContext* Gfx_Immediate_Context(RenderDevice* dev) 
	{
		return &dev->context;
	}

	TextureHandle Gfx_Texture_Load(RenderDevice* dev, const char* filename, TextureType::type type)
	{
		if( type != TextureType::Tex2D )
		{
			Log::error("Only loading simple 2D textures is currently supported.");
			return InvalidResourceHandle();
		}

		Image img;
		if( img.load_dds(filename) )
		{
			TextureGL res;

			res.descr = TextureDescr::create_2D(img.width(), img.height(), img.format(), false, false);

			glPixelStorei ( GL_UNPACK_ALIGNMENT, 1 );
			glGenTextures(1, &res.native);
			glBindTexture(GL_TEXTURE_2D, res.native);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img.width(), img.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, img.data());
			glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
			glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

			return dev->resources.textures.push(res);
		}
		else
		{
			return InvalidResourceHandle();
		}
	}

	void Gfx_PixelShader_Destroy(RenderDevice* dev, PixelShaderHandle& h)
	{
		R_UNUSED(h);
		R_UNUSED(dev);
		// TODO
	}

	void Gfx_IndexBuffer_Destroy(RenderDevice* dev, IndexBufferHandle h)
	{
		R_UNUSED(h);
		R_UNUSED(dev);
		// TODO
	}

	void Gfx_BlendState_Destroy(RenderDevice* dev, BlendStateHandle h)
	{
		R_UNUSED(h);
		R_UNUSED(dev);
		// TODO
	}

	void Gfx_Technique_Destroy(RenderDevice* /*dev*/, TechniqueHandle /*h*/)
	{
		// TODO
	}

	void Gfx_Texture_Destroy(RenderDevice* dev, TextureHandle& h)
	{
		R_UNUSED(h);
		R_UNUSED(dev);
		// TODO
	}

	void Gfx_DestroyDevice(RenderDevice* dev)
	{
#if defined(RUSH_PLATFORM_WINDOWS)
		wglMakeCurrent(NULL, NULL);
		wglDeleteContext(dev->glcontext);
		ReleaseDC(dev->hwnd, dev->hdc);
#endif//RUSH_PLATFORM_WINDOWS
		delete dev;
	}

	RenderDevice* Gfx_CreateDevice(Window* window, const RenderDeviceConfig& cfg) 
	{
		RenderDevice* dev = new RenderDevice;

		dev->config = cfg;
		dev->window = window;

#if defined(RUSH_PLATFORM_WINDOWS)
		PIXELFORMATDESCRIPTOR pfd = 
		{
			sizeof (PIXELFORMATDESCRIPTOR), 1,
			PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
			PFD_TYPE_RGBA, 24 /*colour*/,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			24 /*depth*/, 8 /*stencil*/,
			0, PFD_MAIN_PLANE, 0, 0, 0, 0
		};

		dev->hwnd = *(HWND*)window->native_window_handle();
		dev->hdc = GetDC(dev->hwnd);
		int pf = ChoosePixelFormat(dev->hdc, &pfd);
		SetPixelFormat(dev->hdc, pf, &pfd);
		dev->glcontext = wglCreateContext(dev->hdc);
		wglMakeCurrent(dev->hdc, dev->glcontext);

		get_gl_proc(glBindBuffer, "glBindBuffer");
		get_gl_proc(glCreateShader, "glCreateShader");
		get_gl_proc(glShaderSource, "glShaderSource");
		get_gl_proc(glAttachShader, "glAttachShader");
		get_gl_proc(glCompileShader, "glCompileShader");
		get_gl_proc(glGetProgramiv, "glGetProgramiv");
		get_gl_proc(glGenBuffers, "glGenBuffers");
		get_gl_proc(glBufferData, "glBufferData");
		get_gl_proc(glGetShaderiv, "glGetShaderiv");
		get_gl_proc(glGetShaderInfoLog, "glGetShaderInfoLog");
		get_gl_proc(glDeleteShader, "glDeleteShader");
		get_gl_proc(glBufferSubData, "glBufferSubData");
		get_gl_proc(glUseProgram, "glUseProgram");
		get_gl_proc(glDisableVertexAttribArray, "glDisableVertexAttribArray");
		get_gl_proc(glEnableVertexAttribArray, "glEnableVertexAttribArray");
		get_gl_proc(glVertexAttribPointer, "glVertexAttribPointer");
		get_gl_proc(glActiveTexture, "glActiveTexture");
		get_gl_proc(glUniform1i, "glUniform1i");
		get_gl_proc(glUniformMatrix4fv, "glUniformMatrix4fv");
		get_gl_proc(glUniform4fv, "glUniform4fv");
		get_gl_proc(glBlendEquation, "glBlendEquation");
		get_gl_proc(glCreateProgram, "glCreateProgram");
		get_gl_proc(glLinkProgram, "glLinkProgram");
		get_gl_proc(glGetProgramInfoLog, "glGetProgramInfoLog");
		get_gl_proc(glDeleteProgram, "glDeleteProgram");
		get_gl_proc(glGetUniformLocation, "glGetUniformLocation");
		get_gl_proc(glGetAttribLocation, "glGetAttribLocation");
		get_gl_proc(wglSwapIntervalEXT, "wglSwapIntervalEXT");
		get_gl_proc(glBlendEquationSeparate, "glBlendEquationSeparate");
		get_gl_proc(glBlendFuncSeparate, "glBlendFuncSeparate");
#endif //RUSH_PLATFORM_WINDOWS

		dev->context.resources = &dev->resources;
		return dev;
	}

	DepthStencilStateHandle Gfx_DepthStencilState_Create(RenderDevice* dev, const DepthStencilStateDescr& descr)
	{
		R_UNUSED(dev);
		R_UNUSED(descr);
		return InvalidResourceHandle(); // TODO
	}

	VertexShaderHandle Gfx_VertexShader_Create(RenderDevice* dev, const ShaderCode& code)
	{
		VertexShaderGL res;
		res.native = gl_compile_shader(code.data(), ShaderType::Vertex);
		if( res.native )
		{
			return dev->resources.vertex_shaders.push(res);
		}
		else
		{
			return InvalidResourceHandle();
		}
	}    

	PixelShaderHandle Gfx_PixelShader_Create(RenderDevice* dev, const ShaderCode& code)
	{
		PixelShaderGL res;
		res.native = gl_compile_shader(code.data(), ShaderType::Pixel);
		if( res.native )
		{
			return dev->resources.pixel_shaders.push(res);
		}
		else
		{
			return InvalidResourceHandle();
		}
	}

	VertexBufferHandle Gfx_VertexBuffer_Create(RenderDevice* dev, const VertexBufferDescr& vb, const void* data)
	{
		R_ASSERT_MSG((vb.mode == BufferMode::WriteOnce && data==NULL)==false,  "Static buffers must provide valid initial data.");

		VertexBufferGL res;

		res.descr = vb;

		glGenBuffers(1, &res.native);
		glBindBuffer(GL_ARRAY_BUFFER, res.native);
		glBufferData(GL_ARRAY_BUFFER, vb.size, data, gl_convert(vb.mode));

		return dev->resources.vertex_buffers.push(res);
	}

	VertexFormatHandle Gfx_VertexFormat_Create(RenderDevice* dev, const VertexFormatDescr& fmt)
	{
		VertexFormatGL res;
		res.descr = fmt;
		return dev->resources.vertex_formats.push(res);
	}

	BlendStateHandle Gfx_BlendState_Create(RenderDevice* dev, const BlendStateDescr& descr)
	{
		BlendStateGL res;
		res.descr = descr;
		return dev->resources.blend_states.push(res);
	}

	SamplerStateHandle Gfx_SamplerState_Create(RenderDevice* dev, const SamplerStateDescr& descr)
	{
		SamplerStateGL res;
		res.descr = descr;
		return dev->resources.sampler_states.push(res);
	}

	TechniqueHandle Gfx_Technique_Create(RenderDevice* dev, const TechniqueDescr& descr)
	{
		GLint ps = dev->resources.pixel_shaders[descr.ps].native;
		GLint vs = dev->resources.vertex_shaders[descr.vs].native;

		const VertexFormatDescr& vf = dev->resources.vertex_formats[descr.vf].descr;

		GLuint programObject = glCreateProgram();
		if ( programObject == 0 ) return InvalidResourceHandle();

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
			return InvalidResourceHandle();
		}

		TechniqueGL res;

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
						TechniqueGL::UniformBinding b;
						b.index = location;
						b.data  = item.data;
						b.count = item.count;
						res.uniforms.push_back(b);
					}
					else if( item.type == ShaderConstantBindings::BindingType_Sampler )
					{
						TechniqueGL::SamplerBinding b;
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
				TechniqueGL::AttributeBinding b;
				b.stream = elem.stream;
				b.location = location;
				b.type = vertex_data_type_to_gl(elem.type);
				b.count = vertex_data_type_to_count(elem.type);
				b.normalized = vertex_data_type_to_normalized(elem.type);
				b.stride = vf.stream_stride(elem.stream);
				b.offset = elem.offset;
				res.attributes.push_back(b);
			}
		}

		return dev->resources.techniques.push(res);
	}

	IndexBufferHandle Gfx_IndexBuffer_Create(RenderDevice* dev, const IndexBufferDescr& ib, const void* data)
	{
		R_ASSERT_MSG((ib.mode == BufferMode::WriteOnce && data==NULL)==false,  "Static buffers must provide valid initial data.");

		IndexBufferGL res;

		res.descr = ib;

		glGenBuffers(1, &res.native);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, res.native);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, ib.size, data, gl_convert(ib.mode));

		return dev->resources.index_buffers.push(res);
	}    

	TextureHandle Gfx_Texture_Create(RenderDevice* dev, const TextureDescr& tex, const void* data, size_t size)
	{
		R_UNUSED(size);

		TextureGL res;

		res.descr = tex;

		glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );
		glGenTextures(1, &res.native);
		glBindTexture(GL_TEXTURE_2D, res.native);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tex.width, tex.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

		return dev->resources.textures.push(res);
	}

} // namespace Rush

#else //RUSH_RENDER_API==RUSH_RENDER_API_GL
char _GLRenderDevice_cpp; // suppress linker warning
#endif //RUSH_RENDER_API==RUSH_RENDER_API_GL

