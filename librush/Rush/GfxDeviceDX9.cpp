#include "GfxDeviceDX9.h"

#if RUSH_RENDER_API == RUSH_RENDER_API_DX9

#include "GfxBuffer.h"
#include "GfxShader.h"
#include "GfxVertexFormat.h"
#include "MathAABB2.h"
#include "MathPixel.h"
#include "UtilAssert.h"
#include "UtilLog.h"
#include "UtilEndian.h"
#include "UtilSafeRelease.h"
#include "UtilResourcePool.h"
#include "Window.h"
#include "Filesystem.h"

#include <d3d9.h>
#include <d3dx9.h>
#include <DxErr.h>

namespace Rush
{
#if defined(_DEBUG)
#define USE_D3D_VALIDATION
#endif //_DEBUG

#ifdef USE_D3D_VALIDATION
    bool validate_d3d_result( HRESULT hr, bool show_error_message=true )
    {					
        if( FAILED(hr) )
        {
            if( show_error_message )
            {
                const char* err_str = DXGetErrorStringA(hr);
                Log::error("D3D Error: %s", err_str);
            }
            return false;
        }	
        else
        {
            return true;
        }
    }
#else //USE_D3D_VALIDATION
    inline bool validate_d3d_result( HRESULT /*hr*/, bool /*show_error_message*/ = true) {return true;}
#endif //USE_D3D_VALIDATION

    //////////////////////////////////////////////////////////////////////////

    template <typename Tx, typename Ty>
    inline static uint32 push_back_return_index(Tx& x, const Ty& y)
    {
        uint32 old_size = x.size();
        x.push_back(y);
        return old_size;
    }

    template <typename T>
    static void release_all(T& arr)
    {
        for( uint32 i=0; i<arr.capacity(); ++i )
        {
            T::HandleType h = T::HandleType(UntypedResourceHandle(i));
            SafeRelease( arr[h].native );
        }
    }

    static inline D3DFORMAT pixel_format_to_dx9(PixelFormat::type type)
    {
        switch(type)
        {
        case PixelFormat::R8G8B8:			return D3DFMT_R8G8B8;
        case PixelFormat::A8R8G8B8:			return D3DFMT_A8R8G8B8;
        case PixelFormat::A16B16G16R16:		return D3DFMT_A16B16G16R16;
        case PixelFormat::A16B16G16R16F:	return D3DFMT_A16B16G16R16F;
        case PixelFormat::A32B32G32R32F:	return D3DFMT_A32B32G32R32F;
        case PixelFormat::G16R16F:			return D3DFMT_G16R16F;
        case PixelFormat::G32R32F:			return D3DFMT_G32R32F;
        case PixelFormat::R16F:				return D3DFMT_R16F;
        case PixelFormat::R32F:				return D3DFMT_R32F;
        case PixelFormat::D24S8:			return D3DFMT_D24S8;
        case PixelFormat::L8:				return D3DFMT_L8;
        case PixelFormat::L16:				return D3DFMT_L16;
        case PixelFormat::DXT1:				return D3DFMT_DXT1;
        case PixelFormat::DXT5:				return D3DFMT_DXT5;
        }

        Log::error("Unsupported PixelFormat encountered in pixel_format_to_dx9()");
        return D3DFMT_UNKNOWN;
    }

    static inline PixelFormat::type pixel_format_from_dx9(D3DFORMAT type)
    {
        switch(type)
        {
        case D3DFMT_R8G8B8:					return PixelFormat::R8G8B8;
        case D3DFMT_X8R8G8B8:				return PixelFormat::A8R8G8B8;
        case D3DFMT_A8R8G8B8:				return PixelFormat::A8R8G8B8;
        case D3DFMT_A16B16G16R16:			return PixelFormat::A16B16G16R16;
        case D3DFMT_A16B16G16R16F:			return PixelFormat::A16B16G16R16F;
        case D3DFMT_A32B32G32R32F:			return PixelFormat::A32B32G32R32F;
        case D3DFMT_G16R16F:				return PixelFormat::G16R16F;
        case D3DFMT_G32R32F:				return PixelFormat::G32R32F;
        case D3DFMT_R16F:					return PixelFormat::R16F;
        case D3DFMT_R32F:					return PixelFormat::R32F;
        case D3DFMT_D24S8:					return PixelFormat::D24S8;
        case D3DFMT_L8:						return PixelFormat::L8;
        case D3DFMT_L16:					return PixelFormat::L16;
        case D3DFMT_DXT1:					return PixelFormat::DXT1;
        case D3DFMT_DXT5:					return PixelFormat::DXT5;
        }

        Log::error("Unsupported D3DFORMAT encountered in pixel_format_from_dx9()");
        return PixelFormat::Unknown;
    }

    static inline D3DDECLTYPE data_type_to_dx9(VertexFormatDescr::DataType::type type)
    {
        switch(type)
        {
        case VertexFormatDescr::DataType::Float1:	return D3DDECLTYPE_FLOAT1;
        case VertexFormatDescr::DataType::Float2:	return D3DDECLTYPE_FLOAT2;
        case VertexFormatDescr::DataType::Float3:	return D3DDECLTYPE_FLOAT3;
        case VertexFormatDescr::DataType::Float4:	return D3DDECLTYPE_FLOAT4;
        case VertexFormatDescr::DataType::Half2:	return D3DDECLTYPE_FLOAT16_2;
        case VertexFormatDescr::DataType::Half4:	return D3DDECLTYPE_FLOAT16_4;
        case VertexFormatDescr::DataType::Short2:	return D3DDECLTYPE_SHORT2;
        case VertexFormatDescr::DataType::Short2N:	return D3DDECLTYPE_SHORT2N;
        case VertexFormatDescr::DataType::UByte4:	return D3DDECLTYPE_UBYTE4;
        case VertexFormatDescr::DataType::Dec3N:	return D3DDECLTYPE_DEC3N;
        case VertexFormatDescr::DataType::Colour:	return D3DDECLTYPE_UBYTE4N;
        }

        return (D3DDECLTYPE)-1;
    }

    static inline uint8 usage_type_to_dx9(VertexFormatDescr::UsageType::type type)
    {		
        switch(type)
        {			
        case VertexFormatDescr::UsageType::Position:	return D3DDECLUSAGE_POSITION;
        case VertexFormatDescr::UsageType::Normal:		return D3DDECLUSAGE_NORMAL;
        case VertexFormatDescr::UsageType::Binormal:	return D3DDECLUSAGE_BINORMAL;
        case VertexFormatDescr::UsageType::Tangent:		return D3DDECLUSAGE_TANGENT;
        case VertexFormatDescr::UsageType::Colour:		return D3DDECLUSAGE_COLOR;
        case VertexFormatDescr::UsageType::Texcoord:	return D3DDECLUSAGE_TEXCOORD;
        }

        return 0;
    }

    static inline uint32 primitive_type_to_count(PrimitiveType type, uint32 count)
    {
        switch( type )
        {
        case PrimitiveTypes::TriangleList:	return count/3;
        case PrimitiveTypes::TriangleStrip:	return count-2;
        case PrimitiveTypes::PointList:		return count;
        case PrimitiveTypes::LineList:		return count/2;
        case PrimitiveTypes::LineStrip:		return count-1;
        }

        return 0;
    }

    static inline uint32 buffer_mode_to_d3d_usage_flags(BufferMode::type mode)
    {
        uint32 val[BufferMode::_num_elements];

        val[BufferMode::WriteOnce]		= D3DUSAGE_WRITEONLY;
        val[BufferMode::WriteOften]		= D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC;
        val[BufferMode::WriteRarely]	= D3DUSAGE_DYNAMIC;

        return val[mode];
    }

    static inline uint32 buffer_mode_to_d3d_lock_flags(BufferMode::type mode)
    {
        uint32 val[BufferMode::_num_elements];

        val[BufferMode::WriteOnce]			= 0;
        val[BufferMode::WriteOften]			= D3DLOCK_DISCARD;
        val[BufferMode::WriteRarely]		= 0;

        return val[mode];
    }

    static inline D3DBLENDOP blendop_to_dx9(BlendOp::type op)
    {
        switch(op)
        {
        case BlendOp::Add:			return D3DBLENDOP_ADD;
        case BlendOp::Subtract:		return D3DBLENDOP_SUBTRACT;
        case BlendOp::RevSubtract:	return D3DBLENDOP_REVSUBTRACT;
        case BlendOp::Min:			return D3DBLENDOP_MIN;
        case BlendOp::Max:			return D3DBLENDOP_MAX;
        }

        return (D3DBLENDOP)0;
    }

    static inline D3DBLEND blend_to_dx9(Blend::type blend)
    {
        switch(blend)
        {
        case Blend::Zero:				return D3DBLEND_ZERO;
        case Blend::One:				return D3DBLEND_ONE;
        case Blend::SrcColour:			return D3DBLEND_SRCCOLOR;
        case Blend::InvSrcColour:		return D3DBLEND_INVSRCCOLOR;
        case Blend::SrcAlpha:			return D3DBLEND_SRCALPHA;
        case Blend::InvSrcAlpha:		return D3DBLEND_INVSRCALPHA;
        case Blend::DestAlpha:			return D3DBLEND_DESTALPHA;
        case Blend::InvDestAlpha:		return D3DBLEND_INVDESTALPHA;
        case Blend::DestColour:			return D3DBLEND_DESTCOLOR;
        case Blend::InvDestColour:		return D3DBLEND_INVDESTCOLOR;
        }

        return (D3DBLEND)0;
    }

    static inline DWORD texture_filter_to_dx9(TextureFilter::type filter)
    {
        switch(filter)
        {
        case TextureFilter::Point:	return D3DTEXF_POINT;
        case TextureFilter::Linear:	return D3DTEXF_LINEAR;
        }
        return D3DTEXF_POINT;
    }

    static inline D3DPRIMITIVETYPE primitive_type_to_dx9(PrimitiveType type)
    {
        switch(type)
        {
        case PrimitiveTypes::PointList:		return D3DPT_POINTLIST;
        case PrimitiveTypes::LineList:		return D3DPT_LINELIST;
        case PrimitiveTypes::LineStrip:		return D3DPT_LINESTRIP;
        case PrimitiveTypes::TriangleList:	return D3DPT_TRIANGLELIST;
        case PrimitiveTypes::TriangleStrip:	return D3DPT_TRIANGLESTRIP;
        }

        return D3DPT_POINTLIST;
    }

    static inline uint32 primitive_count_from_index_count(D3DPRIMITIVETYPE type, uint32 index_count)
    {
        switch( type )
        {
        case D3DPT_TRIANGLELIST:	return index_count/3;
        case D3DPT_TRIANGLESTRIP:	return index_count-2;
        case D3DPT_POINTLIST:		return index_count;
        case D3DPT_LINELIST:		return index_count/2;
        case D3DPT_LINESTRIP:		return index_count-1;
        }

        return 0;
    }

	static inline DWORD compare_func_to_dx9(CompareFunc::type fun)
	{
		switch(fun)
		{
		case CompareFunc::Never: return D3DCMP_NEVER;
		case CompareFunc::Less: return D3DCMP_LESS;
		case CompareFunc::Equal: return D3DCMP_EQUAL;
		case CompareFunc::LessEqual: return D3DCMP_LESSEQUAL;
		case CompareFunc::Greater: return D3DCMP_GREATER;
		case CompareFunc::NotEqual: return D3DCMP_NOTEQUAL;
		case CompareFunc::GreaterEqual: return D3DCMP_GREATEREQUAL;
		case CompareFunc::Always: return D3DCMP_ALWAYS;
		default: return D3DCMP_ALWAYS;
		}
	}

    //////////////////////////////////////////////////////////////////////////

    struct VertexFormatDX9
    {
        inline VertexFormatDX9() 
            : native(NULL)
        {}
        VertexFormatDescr desc;
        IDirect3DVertexDeclaration9* native;
    };

    struct VertexShaderDX9
    {
        inline VertexShaderDX9() 
            : native(NULL)
        {}
        IDirect3DVertexShader9* native;
        ShaderCode code;
    };

    struct PixelShaderDX9
    {
        inline PixelShaderDX9()
            : native(NULL)
        {}
        IDirect3DPixelShader9* native;
        ShaderCode code;
    };

    struct TextureDX9
    {
        inline TextureDX9()
            : native2D(NULL), native3D(NULL), nativeCUBE(NULL), surface(NULL) 
        {}

        TextureDescr desc;
        IDirect3DTexture9* native2D;
        IDirect3DVolumeTexture9* native3D;
        IDirect3DCubeTexture9* nativeCUBE;
        IDirect3DSurface9* surface;
    };

    struct IndexBufferDX9
    {
        inline IndexBufferDX9()
            : native(NULL)
        {}
        IndexBufferDescr desc;
        IDirect3DIndexBuffer9* native;
    };

    struct VertexBufferDX9
    {
        inline VertexBufferDX9()
            : native(NULL)
        {}
        VertexBufferDescr desc;
        IDirect3DVertexBuffer9* native;
    };

    struct BlendStateDX9
    {
        BlendStateDescr desc;
    };

    struct SamplerStateDX9
    {
        SamplerStateDescr desc;
    };

    struct TechniqueDX9
    {
        struct Binding : public ShaderConstantIndex
        {
            const Vector4* data;
        };

        PixelShaderHandle		ps;
        ShaderConstantTable		ps_samplers;
        ShaderConstantTable		ps_constants;
        DynamicArray<Binding>	ps_bindings;

        VertexShaderHandle		vs;
        ShaderConstantTable		vs_samplers;
        ShaderConstantTable		vs_constants;
        DynamicArray<Binding>	vs_bindings;

        VertexFormatHandle		vf;
    };

    template <size_t COUNT>
    static void release_all(ResourcePool<TextureDX9, TextureHandle, COUNT>& arr)
    {
        for( uint32 i=0; i<arr.capacity(); ++i )
        {
            TextureHandle h = TextureHandle(UntypedResourceHandle(i));
            SafeRelease( arr[h].surface );
            SafeRelease( arr[h].native2D );
            SafeRelease( arr[h].native3D );
            SafeRelease( arr[h].nativeCUBE);
        }
    }

    template <size_t COUNT>
    static void release_all(ResourcePool<TechniqueDX9, TechniqueHandle, COUNT>& /*arr*/)
    {
        // nothing to do
    }

    template <size_t COUNT>
    static void release_all(ResourcePool<SamplerStateDX9, SamplerStateHandle, COUNT>& /*arr*/)
    {
        // nothing to do
    }

    template <size_t COUNT>
    static void release_all(ResourcePool<BlendStateDX9, BlendStateHandle, COUNT>& /*arr*/)
    {
        // nothing to do
    }

    //////////////////////////////////////////////////////////////////////////

    inline static bool is_sampler(D3DXPARAMETER_TYPE type)
    {
        switch(type)
        {
        case D3DXPT_SAMPLER:
        case D3DXPT_SAMPLER1D:
        case D3DXPT_SAMPLER2D:
        case D3DXPT_SAMPLER3D:
        case D3DXPT_SAMPLERCUBE:
            return true;

        default:
            return false;
        }
    }

    static void dx9_fill_constant_table(ShaderConstantTable& out_constants, ShaderConstantTable& out_samplers, ID3DXConstantTable* constant_table)
    {
        out_constants.clear();
        out_samplers.clear();

        if( constant_table )
        {
            HRESULT hr = S_OK;

            D3DXCONSTANTTABLE_DESC ct_desc;
            hr = constant_table->GetDesc(&ct_desc);
            validate_d3d_result(hr, true);

            for( uint32 i=0; i<ct_desc.Constants; ++i )
            {
                D3DXHANDLE handle = constant_table->GetConstant(NULL, i);

                D3DXCONSTANT_DESC d3dx_constant_desc;
                UINT count = 1;
                hr = constant_table->GetConstantDesc(handle, &d3dx_constant_desc, &count);	
                validate_d3d_result(hr, true);

                ShaderConstantDescr scd;
                scd.name = String(d3dx_constant_desc.Name);
                scd.register_index = d3dx_constant_desc.RegisterIndex;
                scd.register_count = d3dx_constant_desc.RegisterCount;

                if( is_sampler(d3dx_constant_desc.Type) )
                {
                    out_samplers.push_back(scd);
                }
                else
                {
                    out_constants.push_back(scd);
                }
            }
        }
    }

    class DX9ShaderIncluder : public ID3DXInclude
    {
    public:

        DX9ShaderIncluder( FileSystem* fs, const String& root )
            : m_filesystem(fs)
            , m_root(root)
        {
        }


        STDMETHOD(Open)(THIS_ D3DXINCLUDE_TYPE /*IncludeType*/, LPCSTR pFileName, LPCVOID /*pParentData*/, LPCVOID *ppData, UINT *pBytes)
        {
            String source;
            m_filesystem->string_from_file((m_root + "\\" + String(pFileName)).c_str(), source);
            if( source.size() )
            {
                *ppData = malloc(source.length()+1);
                *pBytes = (UINT)source.length();

                memcpy((void*)*ppData, source.data(), source.length()+1);

                return D3D_OK;
            }
            else
            {
                *ppData = NULL;
                return D3DERR_NOTFOUND;
            }
        }

        STDMETHOD(Close)(THIS_ LPCVOID pData) 
        {
            free((void*)pData);
            return D3D_OK;
        }

    private:

        FileSystem* m_filesystem;
        String      m_root;

    };

    inline char* shader_profile_string(ShaderType::type type)
    {
        switch(type)
        {
        case ShaderType::Vertex:    return "vs_3_0";
        case ShaderType::Pixel:     return "ps_3_0";
        default:                    return "UNKNOWN";
        }
    }

    static bool dx9_compile_shader(ShaderCode& out_shader, const ShaderCode& source, ShaderType::type type, FileSystem* fs=NULL, const char* include_dir=NULL)
    {
        R_ASSERT(source.type == ShaderCodeType_Text);

        HRESULT hr = S_OK;

        ID3DXBuffer* error_buffer = NULL;
        ID3DXBuffer* shader_buffer = NULL;
        ID3DXConstantTable* constant_table = NULL;

        uint32 flags = D3DXSHADER_DEBUG;

        const char* profile = shader_profile_string(type);

        static const char dummy_vertex_shader[] = "float4 vertex() : POSITION { return 1; }";
        static const char dummy_pixel_shader[]  = "float4 pixel() : COLOR { return float4(1,0,1,1); }";

        DX9ShaderIncluder includer(fs, include_dir);        

        hr = D3DXCompileShader(source.data(), (UINT)source.size(), NULL, &includer, "main", profile, flags, &shader_buffer, &error_buffer, &constant_table );

        if( hr!=S_OK && error_buffer && error_buffer->GetBufferPointer() )
        {
            char* tmp = (char*)error_buffer->GetBufferPointer();
            R_ERROR_MESSAGE_BOX(tmp, "Shader compile error");   

            shader_buffer = NULL;
            error_buffer = NULL;

            //create dummy shader
            if( type == ShaderType::Pixel )
            {
                hr = D3DXCompileShader(
                    dummy_pixel_shader, uint32(strlen(dummy_pixel_shader)), NULL, NULL, "pixel", "ps_2_0", 
                    flags, &shader_buffer, &error_buffer, NULL );
                validate_d3d_result(hr, true);
            }
            else if( type == ShaderType::Vertex )
            {
                hr = D3DXCompileShader(
                    dummy_vertex_shader, uint32(strlen(dummy_vertex_shader)), NULL, NULL, "vertex", "vs_2_0", 
                    flags, &shader_buffer, &error_buffer, NULL );
                validate_d3d_result(hr, true);
            }
        }

        out_shader.type = ShaderCodeType_Binary;
        out_shader.clear();
        uint32 size = shader_buffer->GetBufferSize();   
        if( size )
        {
            out_shader.resize(size);
            memcpy(out_shader.data(), shader_buffer->GetBufferPointer(), size);
        }

        if(error_buffer) error_buffer->Release();
        if(shader_buffer) shader_buffer->Release();
        if(constant_table) constant_table->Release();

        return true;
    }

    struct RenderResourcesDX9
    {
        #define RUSH_DECLARE_RESOURCE_POOL(native, handle, count, name)\
        ResourcePool<native, handle, count> name;\
        inline native& get(handle h) { return name[h]; }
        RUSH_DECLARE_RESOURCE_POOL(VertexFormatDX9,           VertexFormatHandle,         GfxMaxVertexFormats,        vertex_formats);
        RUSH_DECLARE_RESOURCE_POOL(VertexShaderDX9,           VertexShaderHandle,         GfxMaxVertexShaders,        vertex_shaders);
        RUSH_DECLARE_RESOURCE_POOL(PixelShaderDX9,            PixelShaderHandle,          GfxMaxPixelShaders,         pixel_shaders);
        RUSH_DECLARE_RESOURCE_POOL(TextureDX9,                TextureHandle,              GfxMaxTextures,             textures);
        RUSH_DECLARE_RESOURCE_POOL(IndexBufferDX9,            IndexBufferHandle,          GfxMaxIndexBuffers,         index_buffers)
        RUSH_DECLARE_RESOURCE_POOL(VertexBufferDX9,           VertexBufferHandle,         GfxMaxVertexBuffers,        vertex_buffers);
        RUSH_DECLARE_RESOURCE_POOL(SamplerStateDX9,           SamplerStateHandle,         GfxMaxSamplerStates,        sampler_states);
        RUSH_DECLARE_RESOURCE_POOL(BlendStateDX9,             BlendStateHandle,           GfxMaxBlendStates,          blend_states);
        RUSH_DECLARE_RESOURCE_POOL(TechniqueDX9,              TechniqueHandle,            GfxMaxTechniques,           techniques);
        RUSH_DECLARE_RESOURCE_POOL(DepthStencilStateDescr,    DepthStencilStateHandle,    GfxMaxDepthStencilStates,   depth_stencil_states);
        #undef RUSH_DECLARE_RESOURCE_POOL
    };

    class RenderContext
    {
    public:

        enum
        {
            MaxTextures      = 16,
            MaxVertexStreams = 8
        };

        IDirect3DDevice9*   dx9device;
        RenderResourcesDX9* resources;
        D3DPRIMITIVETYPE    primtype;

        VertexFormatHandle  vertex_format;
        IndexBufferHandle   index_stream;
        VertexBufferHandle  vertex_streams[MaxVertexStreams];
        PixelShaderHandle   pixel_shader;
        VertexShaderHandle  vertex_shader;
        TextureHandle       ps_textures[MaxTextures];
        SamplerStateHandle  ps_samplers[MaxTextures];

        BlendStateHandle    blend_state;
        TechniqueHandle     technique;
    };

    class RenderDevice
    {
    public:

        RenderDeviceConfig  config;
        Window*             window;
        IDirect3D9*         d3d9;
        IDirect3D9Ex*       d3d9ex;
        IDirect3DDevice9*   d3d9device;
        IDirect3DSurface9*  default_backbuffer;
        IDirect3DSurface9*  default_depthbuffer;
        ReferenceCount      refct;
        RenderResourcesDX9* resources;
        RenderContext*      default_context;
    };

    RenderDevice* Gfx_CreateDevice(Window* window, const RenderDeviceConfig& cfg) 
    {
        RenderDevice* dev = new RenderDevice;

        rush_memset(dev, 0, sizeof(RenderDevice));

		dev->window = window;
        dev->config = cfg;
        dev->d3d9 = NULL;
        dev->d3d9ex = NULL;

        // create d3d9

        HRESULT hr = Direct3DCreate9Ex( D3D_SDK_VERSION, &dev->d3d9ex);
        if( SUCCEEDED(hr) == false )
        {
            dev->d3d9 = Direct3DCreate9(D3D_SDK_VERSION);
        }


        // create device

    
        D3DDISPLAYMODE dm = {0};
        D3DPRESENT_PARAMETERS pp = {0};

        uint32      adapter = D3DADAPTER_DEFAULT;
        D3DDEVTYPE  type    = D3DDEVTYPE_HAL;

        uint32 bb_max_width  = cfg.back_buffer_width;
        uint32 bb_max_height = cfg.back_buffer_height;

        R_ASSERT_MSG(bb_max_width>=4 && bb_max_height>=4, "Back buffer configuration is incorrect.");

        bb_max_width  = rush_max(uint32(4),bb_max_width);
        bb_max_height = rush_max(uint32(4),bb_max_height);

        IDirect3D9* d3d = dev->d3d9ex ? dev->d3d9ex : dev->d3d9;

        if( cfg.use_nvperfhud )
        {
            for( uint32 adapter_ctr=0; adapter_ctr<d3d->GetAdapterCount(); ++adapter_ctr )
            {
                D3DADAPTER_IDENTIFIER9 adapter_desc;
                HRESULT Res;
                Res = d3d->GetAdapterIdentifier(adapter_ctr,0,&adapter_desc);
                if(strstr(adapter_desc.Description,"PerfHUD") != 0)
                {
                    adapter = adapter_ctr;
                    type = D3DDEVTYPE_REF;
                    break;
                }
            }
        }

        d3d->GetAdapterDisplayMode( D3DADAPTER_DEFAULT, &dm );

        D3DMULTISAMPLE_TYPE multisampling_type = D3DMULTISAMPLE_NONE;

        DWORD multisampling_quality_levels = 0;
        d3d->CheckDeviceMultiSampleType(adapter, type, dm.Format, TRUE, multisampling_type, &multisampling_quality_levels);

        HWND hwnd = *((HWND*)window->native_window_handle());

        pp.BackBufferWidth              = bb_max_width;
        pp.BackBufferHeight             = bb_max_height;
        pp.BackBufferFormat             = dm.Format;
        pp.BackBufferCount              = 1;

        pp.MultiSampleType              = multisampling_type;
        pp.MultiSampleQuality           = multisampling_quality_levels-1;

        pp.SwapEffect                   = D3DSWAPEFFECT_DISCARD;
        pp.hDeviceWindow                = hwnd;
        pp.Windowed                     = TRUE;
        pp.EnableAutoDepthStencil       = TRUE;
        pp.AutoDepthStencilFormat       = D3DFMT_D24S8;
        pp.Flags                        = 0;

        pp.FullScreen_RefreshRateInHz   = 0;

        if( cfg.use_vertical_sync )
        {
            pp.PresentationInterval = D3DPRESENT_INTERVAL_ONE;
        }
        else
        {
            pp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
        }

        uint32 flags = D3DCREATE_HARDWARE_VERTEXPROCESSING;

        hr = d3d->CreateDevice(adapter, type, hwnd, flags, &pp, &dev->d3d9device);
        R_ASSERT(hr==S_OK);
        validate_d3d_result(hr);        

        hr = dev->d3d9device->GetBackBuffer(0,0,D3DBACKBUFFER_TYPE_MONO, &dev->default_backbuffer);
        R_ASSERT(hr==S_OK);
        validate_d3d_result(hr);

        hr = dev->d3d9device->GetDepthStencilSurface(&dev->default_depthbuffer);
        R_ASSERT(hr==S_OK);
        validate_d3d_result(hr);

        // create resources

        dev->resources = new RenderResourcesDX9;
        dev->default_context = new RenderContext;
        dev->default_context->resources = dev->resources;
        dev->default_context->dx9device = dev->d3d9device;

        // setup some common states

        for( uint32 i=0; i<RenderContext::MaxTextures; ++i )
        {
            dev->d3d9device->SetSamplerState(i, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
            dev->d3d9device->SetSamplerState(i, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
            dev->d3d9device->SetSamplerState(i, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
        }

        dev->d3d9device->SetRenderState(D3DRS_LASTPIXEL, FALSE);
		dev->d3d9device->SetRenderState(D3DRS_SCISSORTESTENABLE, TRUE);

        // all done

        return dev;
    }

    void Gfx_DestroyDevice(RenderDevice* dev)
    {
        delete dev->default_context;        
        delete dev->resources;

        SafeRelease(dev->default_backbuffer);
        SafeRelease(dev->default_depthbuffer);
        SafeRelease(dev->d3d9device);
        SafeRelease(dev->d3d9);
        SafeRelease(dev->d3d9ex);

        delete dev;
    }

    void Gfx_AddReference(RenderDevice* dev)
    {
        dev->refct.add_reference();
    }

    void Gfx_RemoveReference(RenderDevice* dev)
    {
        dev->refct.remove_reference();
    }

    void Gfx_FrameBegin(RenderDevice* dev)
    {
        dev->d3d9device->BeginScene();
    }

    void Gfx_FrameEnd(RenderDevice* dev)
    {
        dev->d3d9device->EndScene();
    }

    void Gfx_Clear(RenderContext* rc, ColourRGBA8 colour, ClearFlags::type clear_flags, float depth, uint32 stencil)
    {
        uint32 flags = 0;

        if( clear_flags & ClearFlags::Colour  ) flags |= D3DCLEAR_TARGET;
        if( clear_flags & ClearFlags::Depth   ) flags |= D3DCLEAR_ZBUFFER;
        if( clear_flags & ClearFlags::Stencil ) flags |= D3DCLEAR_STENCIL;

        ColourRGBA8 colour_bgra(colour.b, colour.g, colour.r, colour.a);

        HRESULT hr = rc->dx9device->Clear(0, NULL, flags, colour_bgra, depth, stencil);

        validate_d3d_result(hr);
    }

    void Gfx_Present(RenderDevice* dev)
    {
		RECT rc;
		rc.left = 0;
		rc.top = 0;
		rc.right = dev->window->width();
		rc.bottom = dev->window->height();

        HRESULT hr= dev->d3d9device->Present(&rc,0,0,0);

        validate_d3d_result(hr);
    }

    // vertex format
    VertexFormatHandle Gfx_VertexFormat_Create(RenderDevice* dev, const VertexFormatDescr& fmt) 
    { 
        const uint32 max_vertex_elements = 128;
        StaticArray<D3DVERTEXELEMENT9, max_vertex_elements> elements_dx9;

        for( uint32 i=0; i<fmt.num_elements(); ++i )
        {
            const VertexFormatDescr::Element& elem = fmt.element(i);

            D3DDECLTYPE type_dx9    = data_type_to_dx9(elem.type);
            uint8 usage_dx9         = usage_type_to_dx9(elem.usage);

            D3DVERTEXELEMENT9 elem_dx9 = { elem.stream, elem.offset, (BYTE)type_dx9, D3DDECLMETHOD_DEFAULT, usage_dx9, elem.index };

            elements_dx9[i] = elem_dx9;
        }

        const D3DVERTEXELEMENT9 VDECL_TERMINATOR = {0xFF,0,D3DDECLTYPE_UNUSED,0,0,0};

        elements_dx9[fmt.num_elements()] = (VDECL_TERMINATOR);

        IDirect3DVertexDeclaration9* d3d9_fmt = NULL;

        HRESULT hr = dev->d3d9device->CreateVertexDeclaration(elements_dx9.data, &d3d9_fmt);

        validate_d3d_result(hr);
            
        if( d3d9_fmt )
        {
            VertexFormatDX9 res;
            res.desc = fmt;
            res.native = d3d9_fmt;
            return dev->resources->vertex_formats.push(res);
        }
        else
        {
            return InvalidResourceHandle();
        }
    }

    void Gfx_VertexFormat_Destroy(RenderDevice* dev, VertexFormatHandle h)
    {
        if( h.valid() )
        {
            SafeRelease(dev->resources->get(h).native);
            dev->resources->vertex_formats.remove(h);
        }
    }

    // vertex shader
    VertexShaderHandle Gfx_VertexShader_Create(RenderDevice* dev, const ShaderCode& code) 
    { 
        VertexShaderDX9 res;

        if( code.type == ShaderCodeType_Binary )
        {
            res.code = code;
        }
        else
        {
            // TODO: handle includes
            if( dx9_compile_shader(res.code, code, ShaderType::Vertex, NULL, NULL) == false )
            {
                return InvalidResourceHandle();
            }
        }

        HRESULT hr = dev->d3d9device->CreateVertexShader((DWORD*)res.code.data(), &res.native);
        if( hr==S_OK )
        {
            return dev->resources->vertex_shaders.push(res);
        }
        else
        {
            return InvalidResourceHandle();
        }
    }

    void Gfx_VertexShader_Destroy(RenderDevice* dev, VertexShaderHandle& h)
    {
        if( h.valid() )
        {
            SafeRelease(dev->resources->vertex_shaders[h].native);
            dev->resources->vertex_shaders.remove(h);
        }
    };

    // pixel shader
    PixelShaderHandle Gfx_PixelShader_Create(RenderDevice* dev, const ShaderCode& code)
    { 
        PixelShaderDX9 res;

        if( code.type == ShaderCodeType_Binary )
        {
            res.code = code;
        }
        else
        {
            // TODO: handle includes
            if( dx9_compile_shader(res.code, code, ShaderType::Pixel, NULL, NULL) == false )
            {
                return InvalidResourceHandle();
            }
        }

        HRESULT hr = dev->d3d9device->CreatePixelShader((DWORD*)res.code.data(), &res.native);
        if( hr==S_OK )
        {
            return dev->resources->pixel_shaders.push(res);
        }
        else
        {
            return InvalidResourceHandle();
        }
    }

    void Gfx_PixelShader_Destroy(RenderDevice* dev, PixelShaderHandle& h)
    {
        if( h.valid() )
        {
            SafeRelease(dev->resources->pixel_shaders[h].native);
            dev->resources->pixel_shaders.remove(h);
        }
    };

    // technique
    TechniqueHandle Gfx_Technique_Create(RenderDevice* dev, const TechniqueDescr& descr)
    { 
        if( descr.ps.valid()==false ) return InvalidResourceHandle();
        if( descr.vs.valid()==false ) return InvalidResourceHandle();
        if( descr.vf.valid()==false ) return InvalidResourceHandle();

        TechniqueDX9 res;

        res.ps = descr.ps;
        res.vs = descr.vs;
        res.vf = descr.vf;

        ID3DXConstantTable* vsct = NULL;
        ID3DXConstantTable* psct = NULL;

        const VertexShaderDX9& vs = dev->resources->vertex_shaders[descr.vs];
        const PixelShaderDX9&  ps = dev->resources->pixel_shaders[descr.ps];

        D3DXGetShaderConstantTable((DWORD*)vs.code.data(), &vsct);
        D3DXGetShaderConstantTable((DWORD*)ps.code.data(), &psct);

        dx9_fill_constant_table(res.vs_constants, res.vs_samplers, vsct);
        dx9_fill_constant_table(res.ps_constants, res.ps_samplers, psct);

        SafeRelease(vsct);
        SafeRelease(psct);

        // create constant bindings [TODO: deal with samplers and vertex streams]

        const ShaderConstantBindings* bindings = descr.bindings;
        if( bindings )
        {
            for( uint32 i=0; i<bindings->items.size(); ++i )
            {
                const ShaderConstantBindings::Item& item = bindings->items[i];

                if( item.type == ShaderConstantBindings::BindingType_Constant )
                {
                    ShaderConstantIndex vs_idx = res.vs_constants.index_from_name(item.name);
                    if( vs_idx.is_valid() )
                    {
                        TechniqueDX9::Binding b;
                        b.index = vs_idx.index;
                        b.count = rush_min(vs_idx.count, item.count);
                        b.data = item.data;
                        res.vs_bindings.push_back(b);
                    }

                    ShaderConstantIndex ps_idx = res.ps_constants.index_from_name(item.name);
                    if( ps_idx.is_valid() )
                    {
                        TechniqueDX9::Binding b;
                        b.index = ps_idx.index;
                        b.count = rush_min(ps_idx.count, item.count);
                        b.data = item.data;
                        res.ps_bindings.push_back(b);
                    }
                }
                else if( item.type == ShaderConstantBindings::BindingType_Sampler )
                {
                    // TODO
                }
            }
        }

        return dev->resources->techniques.push(res);
    }

    void Gfx_Technique_Destroy(RenderDevice* /*dev*/, TechniqueHandle /*h*/)
    {
        // Nothing to do here
    };

    // texture
    TextureHandle Gfx_Texture_Load(RenderDevice* dev, const char* filename, TextureType::type type)
    { 
        D3DXIMAGE_INFO info = {0};

        uint32 mip_levels_flags = D3DX_FROM_FILE;
        D3DFORMAT texture_format_flags = D3DFMT_FROM_FILE;

        TextureDescr tex;
        IDirect3DTexture9* native2D = NULL;
        IDirect3DVolumeTexture9* native3D = NULL;
        IDirect3DCubeTexture9* nativeCUBE = NULL;

        if( type == TextureType::Tex2D )
        {
            HRESULT hr = D3DXCreateTextureFromFileExA(
                dev->d3d9device, filename, D3DX_DEFAULT_NONPOW2, D3DX_DEFAULT_NONPOW2, mip_levels_flags,
                0, texture_format_flags , D3DPOOL_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, 0, &info, NULL, &native2D);
            validate_d3d_result(hr);
            tex = TextureDescr::create_2D(info.Width, info.Height, pixel_format_from_dx9(info.Format));
        }
        else if( type == TextureType::Tex3D )
        {
            HRESULT hr = D3DXCreateVolumeTextureFromFileExA(
                dev->d3d9device, filename, D3DX_DEFAULT_NONPOW2, D3DX_DEFAULT_NONPOW2, D3DX_DEFAULT_NONPOW2, mip_levels_flags,
                0, texture_format_flags , D3DPOOL_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, 0, &info, NULL, &native3D);
            validate_d3d_result(hr);
            tex = TextureDescr::create_3D(info.Width, info.Height, info.Depth, pixel_format_from_dx9(info.Format));
        }
        else if( type == TextureType::TexCUBE )
        {
            HRESULT hr = D3DXCreateCubeTextureFromFileExA(
                dev->d3d9device, filename, D3DX_DEFAULT_NONPOW2, mip_levels_flags,
                0, texture_format_flags , D3DPOOL_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, 0, &info, NULL, &nativeCUBE);
            validate_d3d_result(hr);
            tex = TextureDescr::create_CUBE(info.Width, pixel_format_from_dx9(info.Format));
        }

        if( tex.format == PixelFormat::Unknown )
        {
            Log::warning("Could not load texture '%s'. Perhaps it doesn't exist or uses unsupported format.", filename);
            return InvalidResourceHandle();
        }
        else
        {
            TextureDX9 res;
            res.desc        = tex;
            res.native2D    = native2D;
            res.native3D    = native3D;
            res.nativeCUBE  = nativeCUBE;
            res.surface     = NULL;
            return dev->resources->textures.push(res);
        }
    }
    TextureHandle Gfx_Texture_Create(RenderDevice* dev, const TextureDescr& tex, const void* data, size_t size)
    { 
        D3DFORMAT format_dx9 = pixel_format_to_dx9(tex.format);

        D3DPOOL pool = D3DPOOL_DEFAULT;

        uint32 usage = 0;

        if( tex.dynamic || data )
        {
            usage |= D3DUSAGE_DYNAMIC;
        }

        IDirect3DTexture9* tex2D_dx9 = NULL;
        IDirect3DVolumeTexture9* tex3D_dx9 = NULL;
        IDirect3DCubeTexture9* texCUBE_dx9 = NULL;

        HRESULT hr;
        switch( tex.type )
        {
        case TextureType::Tex2D:
            hr = dev->d3d9device->CreateTexture(tex.width, tex.height, 1, usage, format_dx9, pool, &tex2D_dx9, NULL);
            validate_d3d_result(hr);
            break;
        case TextureType::Tex3D:
            hr = dev->d3d9device->CreateVolumeTexture(tex.width, tex.height, tex.depth, 1, usage, format_dx9, pool, &tex3D_dx9, NULL);
            validate_d3d_result(hr);
            break;
        case TextureType::TexCUBE:
            hr = dev->d3d9device->CreateCubeTexture(tex.width, 1, usage, format_dx9, pool, &texCUBE_dx9, NULL);
            validate_d3d_result(hr);
            break;
        }

        if( data && tex.type == TextureType::Tex2D )
        {
            D3DLOCKED_RECT r;
            hr = tex2D_dx9->LockRect(0, &r, NULL, 0);
            validate_d3d_result(hr);

            R_ASSERT(r.pBits);
            R_ASSERT(tex.width*PixelFormat::bits_per_pixel(tex.format)/8 == (uint32)r.Pitch);
            R_ASSERT(tex.width*tex.height*PixelFormat::bits_per_pixel(tex.format)/8 == size);

            rush_memcpy(r.pBits, data, size);

            tex2D_dx9->UnlockRect(0);
        }
        else if( data )
        {
            Log::warning("Texture filling only supported for 2D textures");
        }

        TextureDX9 res;
        res.desc        = tex;
        res.native2D    = tex2D_dx9;
        res.native3D    = tex3D_dx9;
        res.nativeCUBE  = texCUBE_dx9;
        res.surface     = NULL;

        return dev->resources->textures.push(res);
    }
    
    const TextureDescr& Gfx_Texture_Descr(RenderDevice* dev, TextureHandle h) 
    { 
        if( h.valid() )
        {
            return dev->resources->textures[h].desc;
        }
        else
        {
            static const TextureDescr invalid_texture = TextureDescr::create_2D(1,1,PixelFormat::Unknown,false,false);
            return invalid_texture;
        }
    }

    void Gfx_Texture_Destroy(RenderDevice* dev, TextureHandle& h)
    {
        if( h.valid() )
        {
            SafeRelease(dev->resources->textures[h].surface);
            SafeRelease(dev->resources->textures[h].native2D);
            SafeRelease(dev->resources->textures[h].native3D);
            SafeRelease(dev->resources->textures[h].nativeCUBE);
            dev->resources->textures.remove(h);
        }
    };

    // blend state
    BlendStateHandle Gfx_BlendState_Create(RenderDevice* dev, const BlendStateDescr& descr)
    {
        BlendStateDX9 res;
        res.desc = descr;
        return dev->resources->blend_states.push(res);
    }

    void Gfx_BlendState_Destroy(RenderDevice* dev, BlendStateHandle h)
    {
        dev->resources->blend_states.remove(h);
    };

    // sampler state
    SamplerStateHandle Gfx_SamplerState_Create(RenderDevice* dev, const SamplerStateDescr& descr)
    {
        SamplerStateDX9 res;
        res.desc = descr;
        return dev->resources->sampler_states.push(res);
    };
    void Gfx_SamplerState_Destroy(RenderDevice* dev, SamplerStateHandle h)
    {
        dev->resources->sampler_states.remove(h);
    };

    // render target
    TextureHandle Gfx_RenderTarget_Create(RenderDevice* dev, const TextureDescr& tex)
    {
        (void)dev;
        (void)tex;
        return InvalidResourceHandle(); // TODO
    };

    // vertex buffer
    VertexBufferHandle Gfx_VertexBuffer_Create(RenderDevice* dev, const VertexBufferDescr& vb, const void* data)
    {
        R_ASSERT_MSG((vb.mode == BufferMode::WriteOnce && data==NULL)==false,  "Static buffers must provide valid initial data.");

        uint32  flags   = buffer_mode_to_d3d_usage_flags(vb.mode);
        D3DPOOL pool    = D3DPOOL_DEFAULT;

        uint32 size_in_bytes = vb.count*vb.stride;

        IDirect3DVertexBuffer9* vb_dx9 = NULL;
        HRESULT hr = dev->d3d9device->CreateVertexBuffer(size_in_bytes, flags, 0, pool, &vb_dx9, NULL);
        validate_d3d_result(hr);

        if( data )
        {
            void* locked_data = NULL;
            hr = vb_dx9->Lock(0, vb.size, &locked_data, buffer_mode_to_d3d_lock_flags(vb.mode));
            validate_d3d_result(hr);
            memcpy(locked_data, data, vb.size);
            vb_dx9->Unlock();
        }

        VertexBufferDX9 res;
        res.desc = vb;
        res.native = vb_dx9;

        return dev->resources->vertex_buffers.push(res);
    };

    VertexBufferLock Gfx_VertexBuffer_Lock(RenderDevice* dev, VertexBufferHandle h, uint32 offset, uint32 size) 
    {
        VertexBufferLock lock;

        if( h.valid() )
        {
            lock.size = size;
            lock.handle = h;

            uint32 flags = buffer_mode_to_d3d_lock_flags(dev->resources->vertex_buffers[h].desc.mode);

            IDirect3DVertexBuffer9* vb = dev->resources->vertex_buffers[h].native;
            HRESULT hr = vb->Lock(offset, size, &lock.data, flags);
            validate_d3d_result(hr);
        }

        return lock;
    }

    void Gfx_VertexBuffer_Unlock(RenderDevice* dev, VertexBufferLock& lock)
    {
        if( lock.handle.valid() )
        {
            dev->resources->vertex_buffers[lock.handle].native->Unlock();
            lock.data = NULL;
            lock.size = 0;
        }
    };

    void Gfx_VertexBuffer_Update(RenderDevice* dev, VertexBufferHandle h, void* data, uint32 offset, uint32 size, bool discard)
    {
        (void)discard;

        size = size ? size : dev->resources->vertex_buffers[h].desc.size;      
        VertexBufferLock lock = Gfx_VertexBuffer_Lock(dev, h, offset, size);
        memcpy(lock.data, data, size);
        Gfx_VertexBuffer_Unlock(dev, lock);
    };

    void Gfx_VertexBuffer_Destroy(RenderDevice* dev, VertexBufferHandle h)
    {
        if( h.valid() )
        {
            SafeRelease(dev->resources->vertex_buffers[h].native);
            dev->resources->vertex_buffers.remove(h);
        }
    };

    // index buffer
    IndexBufferHandle Gfx_IndexBuffer_Create(RenderDevice* dev, const IndexBufferDescr& ib, const void* data)
    {
        R_ASSERT_MSG((ib.mode == BufferMode::WriteOnce && data==NULL)==false,  "Static buffers must provide valid initial data.");

        uint32  flags   = buffer_mode_to_d3d_usage_flags(ib.mode);  
        D3DPOOL pool    = D3DPOOL_DEFAULT;

        uint32 stride = IndexFormat::get_stride(ib.type);
        uint32 size_in_bytes = ib.count*stride;

        IDirect3DIndexBuffer9* ib_dx9 = NULL;
        HRESULT hr = dev->d3d9device->CreateIndexBuffer(size_in_bytes, flags, D3DFMT_INDEX32, pool, &ib_dx9, NULL);
        validate_d3d_result(hr);

        if( data )
        {
            void* locked_data = NULL;
            hr = ib_dx9->Lock(0, ib.size, &locked_data, buffer_mode_to_d3d_lock_flags(ib.mode));
            validate_d3d_result(hr);
            memcpy(locked_data, data, ib.size);
            ib_dx9->Unlock();
        }

        IndexBufferDX9 res;
        res.desc = ib;
        res.native = ib_dx9;
        return dev->resources->index_buffers.push(res);
    };

    IndexBufferLock Gfx_IndexBuffer_Lock(RenderDevice* dev, IndexBufferHandle h, uint32 offset, uint32 size) 
    { 
        IndexBufferLock lock;

        if( h.valid() )
        {
            lock.size = size;
            lock.handle = h;

            uint32 flags = buffer_mode_to_d3d_lock_flags(dev->resources->index_buffers[h].desc.mode);

            IDirect3DIndexBuffer9* ib = dev->resources->index_buffers[h].native;
            HRESULT hr = ib->Lock(offset, size, &lock.data, flags);
            validate_d3d_result(hr);
        }

        return lock;
    };

    void Gfx_IndexBuffer_Unlock(RenderDevice* dev, IndexBufferLock& lock)
    {
        if( lock.handle.valid() )
        {
            dev->resources->index_buffers[lock.handle].native->Unlock();
            lock.data = NULL;
            lock.size = 0;
        }
    };

    void Gfx_IndexBuffer_Update(RenderDevice* dev, IndexBufferHandle h, void* data, uint32 offset, uint32 size, bool discard)
    {
        (void)discard;

        size = size ? size : dev->resources->index_buffers[h].desc.size;
        IndexBufferLock lock = Gfx_IndexBuffer_Lock(dev, h, offset, size);
        memcpy(lock.data, data, size);
        Gfx_IndexBuffer_Unlock(dev, lock);
    };

    void Gfx_IndexBuffer_Destroy(RenderDevice* dev, IndexBufferHandle h)
    {
        if( h.valid() )
        {
            SafeRelease(dev->resources->index_buffers[h].native);
            dev->resources->index_buffers.remove(h);
        }
    };

    // context
    RenderContext* Gfx_Immediate_Context(RenderDevice* dev) 
    {
        return dev->default_context; 
    }

	void Gfx_Context_SetScissorRect(RenderContext* rc, const Rect& rect)
	{
		RECT native_rect;

		native_rect.left = rect.left;
		native_rect.top = rect.top;
		native_rect.right = rect.right;
		native_rect.bottom = rect.bottom;

		rc->dx9device->SetScissorRect(&native_rect);
	}

    void Gfx_Context_SetViewport(RenderContext* rc, const Viewport& next)
    {
        D3DVIEWPORT9 native_vp;

        native_vp.X = (DWORD)next.x;
        native_vp.Y = (DWORD)next.y;
        native_vp.Width = (DWORD)next.w;
        native_vp.Height = (DWORD)next.h;
        native_vp.MinZ = next.depth_min;
        native_vp.MaxZ = next.depth_max;

        rc->dx9device->SetViewport(&native_vp);
    }

    void Gfx_Context_SetTechnique(RenderContext* rc, TechniqueHandle h)
    {
        if( h.valid() )
        {
            const TechniqueDX9& t = rc->resources->get(h);

            rc->dx9device->SetVertexDeclaration(rc->resources->get(t.vf).native);
            rc->dx9device->SetVertexShader(rc->resources->get(t.vs).native);
            rc->dx9device->SetPixelShader(rc->resources->get(t.ps).native);

            // TODO: emulate constant buffers on DX9 and OpenGLES2

            for( uint32 i=0; i<t.vs_bindings.size(); ++i )
            {
                rc->dx9device->SetVertexShaderConstantF((UINT)t.vs_bindings[i].index, (const float*)t.vs_bindings[i].data, (UINT)t.vs_bindings[i].count);
            }

            for( uint32 i=0; i<t.ps_bindings.size(); ++i )
            {
                rc->dx9device->SetPixelShaderConstantF((UINT)t.ps_bindings[i].index, (const float*)t.ps_bindings[i].data, (UINT)t.ps_bindings[i].count);
            }
        }
        else
        {
            rc->dx9device->SetVertexDeclaration(NULL);
            rc->dx9device->SetVertexShader(NULL);
            rc->dx9device->SetPixelShader(NULL);
        }

    }

    void Gfx_Context_SetPrimitive(RenderContext* rc, PrimitiveType type)
    {
        rc->primtype = primitive_type_to_dx9(type);
    }

    void Gfx_Context_SetIndexStream(RenderContext* rc, IndexBufferHandle h)
    {
        rc->dx9device->SetIndices(rc->resources->index_buffers[h].native);
    }

    void Gfx_Context_SetVertexStream(RenderContext* rc, uint32 idx, VertexBufferHandle h)
    {
        const VertexBufferDX9& vb = rc->resources->vertex_buffers[h];            
        rc->dx9device->SetStreamSource(idx, vb.native, 0, vb.desc.stride); // TODO: support custom offsets and strides
    }

    void Gfx_Context_SetPixelTexture(RenderContext* rc, uint32 idx, TextureHandle h)
    {
        // todo: support volume and cube textures
        rc->dx9device->SetTexture(idx, rc->resources->get(h).native2D);
    }

    void Gfx_Context_SetPixelSamplerState(RenderContext* rc, uint32 idx, SamplerStateHandle h)
    {
        if( rc->ps_samplers[idx] != h )
        {
            const SamplerStateDescr& next = rc->resources->sampler_states[h].desc;
            rc->dx9device->SetSamplerState(idx, D3DSAMP_MINFILTER, texture_filter_to_dx9(next.filter_min));
            rc->dx9device->SetSamplerState(idx, D3DSAMP_MAGFILTER, texture_filter_to_dx9(next.filter_mag));
            rc->dx9device->SetSamplerState(idx, D3DSAMP_MIPFILTER, texture_filter_to_dx9(next.filter_mip));
            rc->ps_samplers[idx] = h;
        }
    }

    void Gfx_Context_SetBlendState(RenderContext* rc, BlendStateHandle h)
    {
        if( rc->blend_state != h )
        {
            const BlendStateDescr& next = rc->resources->blend_states[h].desc;
            rc->dx9device->SetRenderState(D3DRS_ALPHABLENDENABLE, next.enable);
            rc->dx9device->SetRenderState(D3DRS_SRCBLEND, blend_to_dx9(next.src));
            rc->dx9device->SetRenderState(D3DRS_DESTBLEND, blend_to_dx9(next.dst));
            rc->dx9device->SetRenderState(D3DRS_BLENDOP, blendop_to_dx9(next.op));
            rc->dx9device->SetRenderState(D3DRS_SEPARATEALPHABLENDENABLE, next.alpha_separate);
            if( next.alpha_separate )
            {
                rc->dx9device->SetRenderState(D3DRS_SRCBLENDALPHA, blend_to_dx9(next.alpha_src));
                rc->dx9device->SetRenderState(D3DRS_DESTBLENDALPHA, blend_to_dx9(next.alpha_dst));
                rc->dx9device->SetRenderState(D3DRS_BLENDOPALPHA, blendop_to_dx9(next.alpha_op));
            }
            rc->blend_state = h;
        }
    }

    void Gfx_Context_Draw(RenderContext* rc, uint32 first_vertex, uint32 vertex_count)
    {
        uint32 prim_count = primitive_count_from_index_count(rc->primtype, vertex_count);
        rc->dx9device->DrawPrimitive(rc->primtype, first_vertex, prim_count);
    }

    void Gfx_Context_DrawIndexed(RenderContext* rc, uint32 index_count, uint32 first_index, uint32 base_vertex, uint32 num_vertices)
    {
        uint32 prim_count = primitive_count_from_index_count(rc->primtype, index_count);
        rc->dx9device->DrawIndexedPrimitive(rc->primtype, base_vertex, 0, num_vertices, first_index, prim_count);
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

    DepthStencilStateHandle Gfx_DepthStencilState_Create(RenderDevice* dev, const DepthStencilStateDescr& descr)
    {
        return dev->resources->depth_stencil_states.push(descr);
    }

    void Gfx_DepthStencilState_Destroy(RenderDevice* dev, DepthStencilStateHandle h)
    {
        if(h.valid())
        {
            dev->resources->depth_stencil_states.remove(h);
        }
    }

    void Gfx_Context_SetDepthStencilState(RenderContext* rc, DepthStencilStateHandle next_state)
    {
        if( next_state.valid() )
        {
            const DepthStencilStateDescr& descr = rc->resources->get(next_state);
            rc->dx9device->SetRenderState(D3DRS_ZENABLE, descr.enable?TRUE:FALSE);
            rc->dx9device->SetRenderState(D3DRS_ZWRITEENABLE, descr.write_enable?TRUE:FALSE);
            rc->dx9device->SetRenderState(D3DRS_ZFUNC, compare_func_to_dx9(descr.cmp_func));
            // TODO: other things
        }
    }

}

#else //RUSH_RENDER_API==RUSH_RENDER_API_DX9

char _DX9RenderDevice_cpp; // suppress linker warning

#endif //RUSH_RENDER_API==RUSH_RENDER_API_DX9


