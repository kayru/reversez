#include "GfxDevice.h"

#if RUSH_RENDER_API==RUSH_RENDER_API_DX11

#include "GfxDeviceDX11.h"

#include "Window.h"
#include "UtilLog.h"
#include "UtilAssert.h"
#include "UtilStaticArray.h"
#include "UtilResourcePool.h"
#include "UtilString.h"
#include "UtilSafeRelease.h"
#include "Filesystem.h"
#include "MathPixel.h"

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
    inline bool validate_d3d_result( HRESULT /*hr*/, bool /*show_error_message*/ ) {return true;}
#endif //USE_D3D_VALIDATION

#define D3D_CALL(x) validate_d3d_result(x, true)

    static ID3D11ShaderResourceView* create_srv(ID3D11Device* device, ID3D11Resource* resource)
    {
        ID3D11ShaderResourceView* sr_view = NULL;
        device->CreateShaderResourceView(resource, NULL, &sr_view);
        return sr_view;
    }

    template <typename TO, typename FROM> TO dx11_convert(FROM from);

#define RUSH_DX11_MAP2_CASE_A_B(A,B) case A: return B;
#define RUSH_DX11_MAP2_CASE_B_A(A,B) case B: return A;
#define RUSH_DX11_MAP2_DEFAULT_A(A,B) default: return A;
#define RUSH_DX11_MAP2_DEFAULT_B(A,B) default: return B;
#define RUSH_DX11_MAP2(A,B,C,D,E) template <> B dx11_convert(A type) {switch(type) { C(D,E) }}

#define RUSH_DX11_MAKE_MAP_A_B(A,B,C) RUSH_DX11_MAP2(A,B,C,RUSH_DX11_MAP2_CASE_A_B,RUSH_DX11_MAP2_DEFAULT_B)
#define RUSH_DX11_MAKE_MAP_B_A(A,B,C) RUSH_DX11_MAP2(B,A,C,RUSH_DX11_MAP2_CASE_B_A,RUSH_DX11_MAP2_DEFAULT_A)
#define RUSH_DX11_MAKE_MAP2(A,B,C) RUSH_DX11_MAKE_MAP_A_B(A,B,C) RUSH_DX11_MAKE_MAP_B_A(A,B,C)

#define RUSH_DX11_PIXEL_FORMATS(A,B)\
    A(PixelFormat::DXT1,                        DXGI_FORMAT_BC1_UNORM)\
    A(PixelFormat::DXT5,                        DXGI_FORMAT_BC3_UNORM)\
    A(PixelFormat::A8R8G8B8,                    DXGI_FORMAT_R8G8B8A8_UNORM)\
    B(PixelFormat::Unknown,                     DXGI_FORMAT_UNKNOWN)
    RUSH_DX11_MAKE_MAP2(PixelFormat::type, DXGI_FORMAT, RUSH_DX11_PIXEL_FORMATS);

#define RUSH_DX11_PRIMITIVE_TOPOLOGY(A,B)\
    A(PrimitiveTypes::PointList,                D3D11_PRIMITIVE_TOPOLOGY_POINTLIST)\
    A(PrimitiveTypes::LineList,                 D3D11_PRIMITIVE_TOPOLOGY_LINELIST)\
    A(PrimitiveTypes::LineStrip,                D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP)\
    A(PrimitiveTypes::TriangleList,             D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST)\
    A(PrimitiveTypes::TriangleStrip,            D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP)\
    B(PrimitiveTypes::TriangleList,             D3D11_PRIMITIVE_TOPOLOGY_UNDEFINED)
    RUSH_DX11_MAKE_MAP_A_B(PrimitiveType, D3D11_PRIMITIVE_TOPOLOGY, RUSH_DX11_PRIMITIVE_TOPOLOGY);

#define RUSH_DX11_VERTEX_FORMAT_DESCR_USAGE(A,B)\
    A(VertexFormatDescr::UsageType::Position,   "POSITION")\
    A(VertexFormatDescr::UsageType::Normal,     "NORMAL")\
    A(VertexFormatDescr::UsageType::Binormal,   "BINORMAL")\
    A(VertexFormatDescr::UsageType::Tangent,    "TANGENT")\
    A(VertexFormatDescr::UsageType::Colour,     "COLOR")\
    A(VertexFormatDescr::UsageType::Texcoord,   "TEXCOORD")\
    B(VertexFormatDescr::UsageType::Unused,     "UNUSED")
    RUSH_DX11_MAKE_MAP_A_B(VertexFormatDescr::UsageType::type, const char*, RUSH_DX11_VERTEX_FORMAT_DESCR_USAGE);

#define RUSH_DX11_VERTEX_FORMAT_DATA_TYPE(A,B)\
    A(VertexFormatDescr::DataType::Float1,      DXGI_FORMAT_R32_FLOAT)\
    A(VertexFormatDescr::DataType::Float2,      DXGI_FORMAT_R32G32_FLOAT)\
    A(VertexFormatDescr::DataType::Float3,      DXGI_FORMAT_R32G32B32_FLOAT)\
    A(VertexFormatDescr::DataType::Float4,      DXGI_FORMAT_R32G32B32A32_FLOAT)\
    A(VertexFormatDescr::DataType::Half2,       DXGI_FORMAT_R16G16_FLOAT)\
    A(VertexFormatDescr::DataType::Half4,       DXGI_FORMAT_R16G16B16A16_FLOAT)\
    A(VertexFormatDescr::DataType::Short2,      DXGI_FORMAT_R16G16_SINT)\
    A(VertexFormatDescr::DataType::Short2N,     DXGI_FORMAT_R16G16_SNORM)\
    A(VertexFormatDescr::DataType::UByte4,      DXGI_FORMAT_R8G8B8A8_UINT)\
    A(VertexFormatDescr::DataType::Colour,      DXGI_FORMAT_R8G8B8A8_UNORM)\
    B(VertexFormatDescr::DataType::Unused,      DXGI_FORMAT_UNKNOWN)
    RUSH_DX11_MAKE_MAP_A_B(VertexFormatDescr::DataType::type, DXGI_FORMAT, RUSH_DX11_VERTEX_FORMAT_DATA_TYPE);

#define RUSH_DX11_BLEND_TYPE(A,B)\
    A(Blend::Zero,                              D3D11_BLEND_ZERO)\
    A(Blend::One,                               D3D11_BLEND_ONE)\
    A(Blend::SrcColour,                         D3D11_BLEND_SRC_COLOR)\
    A(Blend::InvSrcColour,                      D3D11_BLEND_INV_SRC_COLOR)\
    A(Blend::SrcAlpha,                          D3D11_BLEND_SRC_ALPHA)\
    A(Blend::InvSrcAlpha,                       D3D11_BLEND_INV_SRC_ALPHA)\
    A(Blend::DestAlpha,                         D3D11_BLEND_DEST_ALPHA)\
    A(Blend::InvDestAlpha,                      D3D11_BLEND_INV_DEST_ALPHA)\
    A(Blend::DestColour,                        D3D11_BLEND_DEST_COLOR)\
    A(Blend::InvDestColour,                     D3D11_BLEND_INV_DEST_COLOR)\
    B(Blend::Zero,                              D3D11_BLEND_ZERO)
    RUSH_DX11_MAKE_MAP_A_B(Blend::type, D3D11_BLEND, RUSH_DX11_BLEND_TYPE);

#define RUSH_DX11_INDEX_FORMAT(A,B)\
    A(IndexFormat::Index16,                     DXGI_FORMAT_R16_UINT)\
    A(IndexFormat::Index32,                     DXGI_FORMAT_R32_UINT)\
    B(IndexFormat::Unknown,                     DXGI_FORMAT_UNKNOWN)
    RUSH_DX11_MAKE_MAP_A_B(IndexFormat::type, DXGI_FORMAT, RUSH_DX11_INDEX_FORMAT);

#define RUSH_DX11_BUFFER_MODE(A,B)\
    A(BufferMode::WriteOnce,                    D3D11_USAGE_IMMUTABLE)\
    A(BufferMode::WriteOften,                   D3D11_USAGE_DYNAMIC)\
    A(BufferMode::WriteRarely,                  D3D11_USAGE_DYNAMIC)\
    B(BufferMode::WriteOnce,                    D3D11_USAGE_DEFAULT)
    RUSH_DX11_MAKE_MAP_A_B(BufferMode::type, D3D11_USAGE, RUSH_DX11_BUFFER_MODE);

#define RUSH_DX11_MAP_MODE(A,B)\
    A(BufferMode::WriteOnce,                    D3D11_MAP_WRITE_DISCARD)\
    A(BufferMode::WriteOften,                   D3D11_MAP_WRITE_DISCARD)\
    A(BufferMode::WriteRarely,                  D3D11_MAP_WRITE_DISCARD)\
    B(BufferMode::WriteRarely,                  D3D11_MAP_WRITE_DISCARD)
    RUSH_DX11_MAKE_MAP_A_B(BufferMode::type, D3D11_MAP, RUSH_DX11_MAP_MODE);

#define RUSH_DX11_CPU_ACCESS(A,B)\
    A(BufferMode::WriteOnce,                    (D3D11_CPU_ACCESS_FLAG)0)\
    A(BufferMode::WriteOften,                   D3D11_CPU_ACCESS_WRITE)\
    A(BufferMode::WriteRarely,                  D3D11_CPU_ACCESS_WRITE)\
    B(BufferMode::WriteRarely,                  (D3D11_CPU_ACCESS_FLAG)0)
    RUSH_DX11_MAKE_MAP_A_B(BufferMode::type, D3D11_CPU_ACCESS_FLAG, RUSH_DX11_CPU_ACCESS);

#define RUSH_DX11_BLEND_OP(A,B)\
    A(BlendOp::Add,                             D3D11_BLEND_OP_ADD)\
    A(BlendOp::Subtract,                        D3D11_BLEND_OP_SUBTRACT)\
    A(BlendOp::RevSubtract,                     D3D11_BLEND_OP_REV_SUBTRACT)\
    A(BlendOp::Min,                             D3D11_BLEND_OP_MIN)\
    A(BlendOp::Max,                             D3D11_BLEND_OP_MAX)\
    B(BlendOp::Add,                             D3D11_BLEND_OP_ADD)
    RUSH_DX11_MAKE_MAP_A_B(BlendOp::type, D3D11_BLEND_OP, RUSH_DX11_BLEND_OP);

#define RUSH_DX11_CMP_FUNC(A,B)\
    A(CompareFunc::Never,                        D3D11_COMPARISON_NEVER)\
    A(CompareFunc::Less,                         D3D11_COMPARISON_LESS)\
    A(CompareFunc::Equal,                        D3D11_COMPARISON_EQUAL)\
    A(CompareFunc::LessEqual,                    D3D11_COMPARISON_LESS_EQUAL)\
    A(CompareFunc::Greater,                      D3D11_COMPARISON_GREATER)\
    A(CompareFunc::NotEqual,                     D3D11_COMPARISON_NOT_EQUAL)\
    A(CompareFunc::GreaterEqual,                 D3D11_COMPARISON_GREATER_EQUAL)\
    A(CompareFunc::Always,                       D3D11_COMPARISON_ALWAYS)\
    B(CompareFunc::Never,                        D3D11_COMPARISON_NEVER)
    RUSH_DX11_MAKE_MAP_A_B(CompareFunc::type, D3D11_COMPARISON_FUNC, RUSH_DX11_CMP_FUNC);

#undef RUSH_DX11_MAP2_CASE_A_B
#undef RUSH_DX11_MAP2_CASE_B_A
#undef RUSH_DX11_MAP2_DEFAULT_A
#undef RUSH_DX11_MAP2_DEFAULT_B
#undef RUSH_DX11_MAP2
#undef RUSH_DX11_MAKE_MAP_A_B
#undef RUSH_DX11_MAKE_MAP_B_A
#undef RUSH_DX11_MAKE_MAP2

    static void dx11_fill_constant_table(ShaderConstantTable& out_constants, ShaderConstantTable& out_samplers, const ShaderCode& bytecode)
    {
        out_constants.clear();
        out_samplers.clear();

        ID3D11ShaderReflection* refl = NULL; 
        D3DReflect( bytecode.data(), bytecode.size(), IID_ID3D11ShaderReflection, (void**)&refl);

        if( refl )
        {
            HRESULT hr = S_OK;
            D3D11_SHADER_DESC refl_desc;
            hr = refl->GetDesc(&refl_desc);

            for( uint32 i=0; i<refl_desc.ConstantBuffers; ++i )
            {
                ID3D11ShaderReflectionConstantBuffer* cb = refl->GetConstantBufferByIndex(i);

                D3D11_SHADER_BUFFER_DESC sb_desc;
                cb->GetDesc(&sb_desc);

                for( uint32 j=0; j<sb_desc.Variables; ++j )
                {
                    ID3D11ShaderReflectionVariable* var = cb->GetVariableByIndex(j);
                    D3D11_SHADER_VARIABLE_DESC var_desc;
                    var->GetDesc(&var_desc);

                    ShaderConstantDescr scd;
                    scd.name = var_desc.Name;
                    scd.register_index = var_desc.StartOffset/16;
                    scd.register_count = var_desc.Size/16;

                    out_constants.push_back(scd);
                }
            }

            for( uint32 i=0; i<refl_desc.BoundResources; ++i )
            {
                D3D11_SHADER_INPUT_BIND_DESC desc;
                refl->GetResourceBindingDesc(i, &desc);
                if( desc.Type == D3D10_SIT_SAMPLER )
                {
                    ShaderConstantDescr scd;
                    scd.name = desc.Name;
                    scd.register_index = desc.BindPoint;
                    scd.register_count = desc.BindCount;
                }
            }

            refl->Release();
        }
    }

    static bool dx11_compile_shader( ShaderCode& out_shader, const ShaderCode& source, const char* entry, const char* profile_str, FileSystem* /*fs*/, const char* /*include_dir*/ )
    {
        LPD3DBLOB code_blob = NULL;
        LPD3DBLOB error_blob = NULL;

        uint32 flags = D3D10_SHADER_DEBUG | D3D10_SHADER_ENABLE_BACKWARDS_COMPATIBILITY;

        HRESULT hr = D3DCompile(source.data(), source.size(), "", NULL, NULL, entry, profile_str, flags, 0, &code_blob, &error_blob);

        out_shader.clear();

        if( hr == S_OK )
        {
            size_t size = code_blob->GetBufferSize();
            out_shader.resize(size);
            memcpy(out_shader.data(), code_blob->GetBufferPointer(), size);
        }

        if( error_blob != NULL )
        {
            char* error_str = (char*)error_blob->GetBufferPointer();
            Log::error("Failed to compile shader\n%s", error_str);
        }

        SafeRelease(code_blob);
        SafeRelease(error_blob);

        if( hr == S_OK )
        {
            return true;
        }
        else
        {
            return false;
        }
    }

    RenderDevice* Gfx_CreateDevice(Window* window, const RenderDeviceConfig& cfg) 
    {
        (void)cfg; // TODO

        RenderDevice* dev = new RenderDevice;

        dev->vsync = cfg.use_vertical_sync ? 1 : 0;

        dev->default_context = new RenderContext;
        dev->default_context->resources = &dev->resources;

        // create d3d11 device and essential resources

        HWND hwnd = *(HWND*)window->native_window_handle();

        DXGI_SWAP_CHAIN_DESC sd;
        ZeroMemory( &sd, sizeof( sd ) );
        sd.BufferCount = 1;
        sd.BufferDesc.Width = window->width();
        sd.BufferDesc.Height = window->height();
        sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        sd.BufferDesc.RefreshRate.Numerator = 60;
        sd.BufferDesc.RefreshRate.Denominator = 1;
        sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        sd.OutputWindow = hwnd;
        sd.SampleDesc.Count = 1;
        sd.SampleDesc.Quality = 0;
        sd.Windowed = TRUE;

        uint32 flags = D3D11_CREATE_DEVICE_SINGLETHREADED;

#ifdef _DEBUG
        flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif //_DEBUG

        D3D_DRIVER_TYPE type = D3D_DRIVER_TYPE_HARDWARE;

        IDXGIAdapter* adapter = NULL;

        if( cfg.use_nvperfhud )
        {
            IDXGIAdapter* enumerated_adapter = NULL;
            IDXGIFactory* factory = NULL;
            D3D_CALL( CreateDXGIFactory(__uuidof(IDXGIFactory),(void**)&factory) );
            for( uint32 i=0; factory->EnumAdapters(i,&enumerated_adapter) != DXGI_ERROR_NOT_FOUND; ++i )
            {
                DXGI_ADAPTER_DESC adapter_desc;
                if(enumerated_adapter->GetDesc(&adapter_desc) != S_OK) 
                {
                    continue;
                }
                if(wcsstr(adapter_desc.Description,L"PerfHUD") != 0) 
                {
                    type = D3D_DRIVER_TYPE_REFERENCE;
                    adapter = enumerated_adapter;
                    break;
                }
            }
            SafeRelease(factory);
        }

        D3D_FEATURE_LEVEL features[] =
        {
            D3D_FEATURE_LEVEL_11_0,
            D3D_FEATURE_LEVEL_10_1,
            D3D_FEATURE_LEVEL_10_0,
            D3D_FEATURE_LEVEL_9_3,
            D3D_FEATURE_LEVEL_9_2,
            D3D_FEATURE_LEVEL_9_1
        };

        const uint32 num_features = sizeof(features) / sizeof(features[0]);

        D3D_FEATURE_LEVEL supported_features = D3D_FEATURE_LEVEL_9_1;

        // Create device

        D3D_CALL( D3D11CreateDeviceAndSwapChain(
            adapter, type, NULL, flags, features, num_features, 
            D3D11_SDK_VERSION, &sd, &dev->swap_chain, &dev->native, 
            &supported_features, &dev->default_context->native) );

        // Get default back buffer

        D3D_CALL( dev->swap_chain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&dev->backbuffer_tex) );
        D3D_CALL( dev->native->CreateRenderTargetView(dev->backbuffer_tex, NULL, &dev->backbuffer_rtv) );

        // Create default depth buffer
        // create depth stencil
        D3D11_TEXTURE2D_DESC depth_stencil_desc;
        depth_stencil_desc.Width = window->width();
        depth_stencil_desc.Height = window->height();
        depth_stencil_desc.MipLevels = 1;
        depth_stencil_desc.ArraySize = 1;
        depth_stencil_desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
        depth_stencil_desc.SampleDesc.Count = 1;
        depth_stencil_desc.SampleDesc.Quality = 0;
        depth_stencil_desc.Usage = D3D11_USAGE_DEFAULT;
        depth_stencil_desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
        depth_stencil_desc.CPUAccessFlags = 0;
        depth_stencil_desc.MiscFlags = 0;

        D3D_CALL( dev->native->CreateTexture2D(&depth_stencil_desc, NULL, &dev->depthstencil_tex) );

        // create depth stencil view
        D3D11_DEPTH_STENCIL_VIEW_DESC depth_stencil_view_desc = CD3D11_DEPTH_STENCIL_VIEW_DESC();
        depth_stencil_view_desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
        depth_stencil_view_desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
        depth_stencil_view_desc.Texture2D.MipSlice = 0;
        depth_stencil_view_desc.Flags = 0;

        D3D_CALL( dev->native->CreateDepthStencilView(dev->depthstencil_tex, &depth_stencil_view_desc, &dev->depthstencil_dsv) );

        // Set-up default Colour and Depth surfaces

        dev->default_context->native->OMSetRenderTargets(1, &dev->backbuffer_rtv, dev->depthstencil_dsv);

        D3D11_VIEWPORT viewport = CD3D11_VIEWPORT(dev->backbuffer_tex, dev->backbuffer_rtv);
        dev->default_context->native->RSSetViewports(1, &viewport);

        dev->default_context->current_rtv = dev->backbuffer_rtv;
        dev->default_context->current_dsv = dev->depthstencil_dsv;

        // create default pixel and vertex constant buffers

        D3D11_BUFFER_DESC desc;

        desc.ByteWidth = MaxShaderConstants*sizeof(Vector4);
        desc.Usage = D3D11_USAGE_DYNAMIC;
        desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        desc.MiscFlags = 0;
        desc.StructureByteStride = 0;

        D3D_CALL(dev->native->CreateBuffer(&desc, NULL, &dev->default_context->ps_cb));
        D3D_CALL(dev->native->CreateBuffer(&desc, NULL, &dev->default_context->vs_cb));

		// Hack -- enable scissor rect state by default
		D3D11_RASTERIZER_DESC rasterizer_desc = CD3D11_RASTERIZER_DESC(CD3D11_DEFAULT());
		rasterizer_desc.ScissorEnable = TRUE;
		dev->native->CreateRasterizerState(&rasterizer_desc, &dev->rasterizer_state);
		dev->default_context->native->RSSetState(dev->rasterizer_state);

        // all done

        return dev;
    }

    void Gfx_DestroyDevice(RenderDevice* dev)
    {
		SafeRelease(dev->rasterizer_state);

        SafeRelease(dev->depthstencil_dsv);
        SafeRelease(dev->depthstencil_tex);

        SafeRelease(dev->backbuffer_rtv);
        SafeRelease(dev->backbuffer_tex);

        SafeRelease(dev->swap_chain);

        SafeRelease(dev->default_context->ps_cb);
        SafeRelease(dev->default_context->vs_cb);

        SafeRelease(dev->default_context->native);

        SafeRelease(dev->native);
        
        delete dev;
    }

    void Gfx_AddReference(RenderDevice* dev)
    {
        dev->refcount.add_reference();
    }
    
    void Gfx_RemoveReference(RenderDevice* dev)
    {
        dev->refcount.remove_reference();
    }

    void Gfx_FrameBegin(RenderDevice* /*dev*/)
    {
        // nothing
    }

    void Gfx_FrameEnd(RenderDevice* /*dev*/)
    {
        // nothing
    }

    void Gfx_Clear(RenderContext* rc, ColourRGBA8 colour, ClearFlags::type clear_flags, float depth, uint32 stencil)
    {
        ColourRGBA colour_fp = colour;
        float colour_array[4] = { colour_fp.r, colour_fp.g, colour_fp.b, colour_fp.a };

        if( (clear_flags & ClearFlags::Colour) && rc->current_rtv )
        {
            rc->native->ClearRenderTargetView(rc->current_rtv, colour_array);
        }

        uint32 ds_clear_flags = 0;

        if(clear_flags & ClearFlags::Depth)     ds_clear_flags |= D3D11_CLEAR_DEPTH;
        if(clear_flags & ClearFlags::Stencil)   ds_clear_flags |= D3D11_CLEAR_STENCIL;

        if( ds_clear_flags && rc->current_dsv )
        {
            rc->native->ClearDepthStencilView(rc->current_dsv, ds_clear_flags, depth, (uint8)stencil);
        }
    }

    void Gfx_Present(RenderDevice* dev)
    {
        D3D_CALL( dev->swap_chain->Present(dev->vsync, 0) );
    }

    const RenderDeviceStats& Gfx_Stats(RenderDevice* /*dev*/)
    { 
        // TODO
        static RenderDeviceStats s; 
        return s; 
    }

    void Gfx_ResetStats(RenderDevice* /*dev*/)
    {
        // TODO
    }

    // vertex format
    VertexFormatHandle Gfx_VertexFormat_Create(RenderDevice* dev, const VertexFormatDescr& fmt) 
    { 
        VertexFormatDX11 res;
        res.descr = fmt;        
        return dev->resources.vertex_formats.push(res);
    }

    void Gfx_VertexFormat_Destroy(RenderDevice* dev, VertexFormatHandle h)
    {
        if( h.valid() )
        {
            dev->resources.vertex_formats.remove(h);
        }
    }

    // vertex shader
    VertexShaderHandle Gfx_VertexShader_Create(RenderDevice* dev, const ShaderCode& code) 
    { 
        ShaderCode compiled_code;
        const ShaderCode* final_code = NULL;

        if( code.type == ShaderCodeType_Text )
        {
            if( dx11_compile_shader(compiled_code, code, "main", "vs_5_0", NULL, NULL) )
            {
                final_code = &compiled_code;
            }
            else
            {
                return InvalidResourceHandle();
            }
        }
        else
        {
            final_code = &code;
        }

        ID3D11VertexShader* d3d11_vsh = NULL;
        dev->native->CreateVertexShader(final_code->data(), final_code->size(), NULL, &d3d11_vsh);
        if( d3d11_vsh )
        {
            VertexShaderDX11 vsh;
            vsh.code = *final_code;
            vsh.native = d3d11_vsh;
            dx11_fill_constant_table(vsh.constants, vsh.samplers, vsh.code);
            return dev->resources.vertex_shaders.push(vsh);
        }
        else
        {
            Log::error("Failed to create vertex shader");
            return InvalidResourceHandle();
        }
    }

    void Gfx_VertexShader_Destroy(RenderDevice* dev, VertexShaderHandle& vsh)
    {
        if( vsh.valid() )
        {
            SafeRelease(dev->resources.vertex_shaders[vsh].native);
            dev->resources.vertex_shaders.remove(vsh);
        }
    }

    // pixel shader
    PixelShaderHandle Gfx_PixelShader_Create(RenderDevice* dev, const ShaderCode& code)
    { 
        ShaderCode compiled_code;
        const ShaderCode* final_code = NULL;

        if( code.type == ShaderCodeType_Text )
        {
            if( dx11_compile_shader(compiled_code, code, "main", "ps_5_0", NULL, NULL) )
            {
                final_code = &compiled_code;
            }
            else
            {
                return InvalidResourceHandle();
            }
        }
        else
        {
            final_code = &code;
        }

        ID3D11PixelShader* d3d11_psh = NULL;
        dev->native->CreatePixelShader(final_code->data(), final_code->size(), NULL, &d3d11_psh);
        if( d3d11_psh )
        {
            PixelShaderDX11 psh;
            psh.code = *final_code;
            psh.native = d3d11_psh;
            dx11_fill_constant_table(psh.constants, psh.samplers, psh.code);
            return dev->resources.pixel_shaders.push(psh);
        }
        else
        {
            Log::error("Failed to create pixel shader");
            return InvalidResourceHandle();
        }
    }

    void Gfx_PixelShader_Destroy(RenderDevice* dev, PixelShaderHandle& h)
    {
        if( h.valid() )
        {
            SafeRelease(dev->resources.pixel_shaders[h].native);
            dev->resources.pixel_shaders.remove(h);
        }
    }

    // technique
    TechniqueHandle Gfx_Technique_Create(RenderDevice* dev, const TechniqueDescr& descr)
    { 
        if( descr.ps.valid()==false ||
            descr.vs.valid()==false ||
            descr.vf.valid()==false ) 
        {
            return InvalidResourceHandle();;
        }

        TechniqueDX11 res;

        res.ps = descr.ps;
        res.vs = descr.vs;
        res.vf = descr.vf;

        const VertexFormatDescr& vf_descr = dev->resources.vertex_formats[descr.vf].descr;

        const uint32 max_vertex_elements = 32;
        StaticArray<D3D11_INPUT_ELEMENT_DESC, max_vertex_elements> elements;
		UINT num_elements = (UINT)vf_descr.num_elements();

        for( uint32 i=0; i<num_elements; ++i )
        {
            const VertexFormatDescr::Element& elem = vf_descr.element(i);
            D3D11_INPUT_ELEMENT_DESC& native_elem = elements[i];
            native_elem.SemanticName = dx11_convert<const char*>(elem.usage);
            native_elem.SemanticIndex = elem.index;
            native_elem.Format = dx11_convert<DXGI_FORMAT>(elem.type);
            native_elem.InputSlot = elem.stream;
            native_elem.AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
            native_elem.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
            native_elem.InstanceDataStepRate = 0;
            elements.current_size++;
        }

        const ShaderCode& vsh_code = dev->resources.vertex_shaders[descr.vs].code;

        D3D_CALL(dev->native->CreateInputLayout(elements.data, num_elements, vsh_code.data(), vsh_code.size(), &res.input_layout));

        const ShaderConstantTable& vs_constants = dev->resources.vertex_shaders[descr.vs].constants;
        const ShaderConstantTable& ps_constants = dev->resources.pixel_shaders[descr.ps].constants;

        if( descr.bindings )
        {
            for( uint32 i=0; i<descr.bindings->items.size(); ++i )
            {
                const ShaderConstantBindings::Item& item = descr.bindings->items[i];

                if( item.type == ShaderConstantBindings::BindingType_Constant )
                {
                    ShaderConstantIndex vs_idx = vs_constants.index_from_name(item.name);
                    if( vs_idx.is_valid() )
                    {
                        TechniqueDX11::Binding b;
                        b.index = vs_idx.index;
                        b.count = rush_min(vs_idx.count, item.count);
                        b.data = item.data;
                        res.vs_bindings.push_back(b);
                    }

                    ShaderConstantIndex ps_idx = ps_constants.index_from_name(item.name);
                    if( ps_idx.is_valid() )
                    {
                        TechniqueDX11::Binding b;
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

        return dev->resources.techniques.push(res);
    }

    void Gfx_Technique_Destroy(RenderDevice* dev, TechniqueHandle h)
    {
        if( h.valid() )
        {
            SafeRelease(dev->resources.techniques[h].input_layout);
            dev->resources.techniques.remove(h);
        }
    }

    // texture
    TextureHandle Gfx_Texture_Load(RenderDevice* dev, const char* filename, TextureType::type type)
    { 
        (void)type; // TODO

        ID3D11Texture2D* tex_dx11 = NULL;
        D3DX11_IMAGE_LOAD_INFO image_load_info;

        D3D_CALL( D3DX11CreateTextureFromFileA(dev->native, filename, &image_load_info, NULL, (ID3D11Resource**)&tex_dx11, NULL) );

        if( tex_dx11 == NULL ) return InvalidResourceHandle();

        D3D11_TEXTURE2D_DESC desc;
        tex_dx11->GetDesc(&desc);

        TextureDescr tex = TextureDescr::create_2D(desc.Width, desc.Height, dx11_convert<PixelFormat::type>(desc.Format), TextureType::Tex2D);

        R_ASSERT( tex.format!=PixelFormat::Unknown );

        TextureDX11 res;

        res.desc = tex;
        res.native2D = tex_dx11;
        res.srv = create_srv(dev->native, tex_dx11);

        return dev->resources.textures.push(res);
    }

    TextureHandle Gfx_Texture_Create(RenderDevice* dev, const TextureDescr& tex, const void* data, size_t /*size*/)
    {
        ID3D11Texture2D* tex_dx11 = NULL;

        DXGI_FORMAT format = dx11_convert<DXGI_FORMAT>(tex.format);
        D3D11_TEXTURE2D_DESC desc = CD3D11_TEXTURE2D_DESC(format, tex.width, tex.height, 1, 1);

        UINT pitch = (tex.width * PixelFormat::bits_per_pixel(tex.format))/8;

        D3D11_SUBRESOURCE_DATA initial_data;
        initial_data.pSysMem = data;
        initial_data.SysMemPitch = pitch;
        initial_data.SysMemSlicePitch = pitch * tex.height;

        D3D_CALL(dev->native->CreateTexture2D(&desc, data?&initial_data:NULL, &tex_dx11));

        TextureDX11 res;

        res.desc = tex;
        res.native2D = tex_dx11;
        res.srv = create_srv(dev->native, tex_dx11);

        return dev->resources.textures.push(res);
    }

    const TextureDescr& Gfx_Texture_Descr(RenderDevice* dev, TextureHandle h)
    {
        return dev->resources.textures[h].desc;
    }
    
    void Gfx_Texture_Destroy(RenderDevice* dev, TextureHandle& h)
    {
        if( h.valid() )
        {
            TextureDX11& t = dev->resources.textures[h];
            SafeRelease(t.rtv);
            SafeRelease(t.srv);
            SafeRelease(t.native2D);
            SafeRelease(t.native3D);
            SafeRelease(t.nativeCUBE);
            dev->resources.textures.remove(h);
        }
    }

    // blend state
    BlendStateHandle Gfx_BlendState_Create(RenderDevice* dev, const BlendStateDescr& descr)
    {
        D3D11_BLEND_DESC desc_dx11;

        desc_dx11.AlphaToCoverageEnable = FALSE;
        desc_dx11.IndependentBlendEnable = FALSE;
        desc_dx11.RenderTarget[0].BlendEnable = descr.enable;
        desc_dx11.RenderTarget[0].SrcBlend = dx11_convert<D3D11_BLEND>(descr.src);
        desc_dx11.RenderTarget[0].DestBlend = dx11_convert<D3D11_BLEND>(descr.dst);
        desc_dx11.RenderTarget[0].BlendOp = dx11_convert<D3D11_BLEND_OP>(descr.op);
        desc_dx11.RenderTarget[0].SrcBlendAlpha = dx11_convert<D3D11_BLEND>(descr.alpha_src);
        desc_dx11.RenderTarget[0].DestBlendAlpha = dx11_convert<D3D11_BLEND>(descr.alpha_dst);
        desc_dx11.RenderTarget[0].BlendOpAlpha = dx11_convert<D3D11_BLEND_OP>(descr.alpha_op);
        desc_dx11.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL; // TODO

        // TODO: support other render targets

        ID3D11BlendState* native = NULL;
        D3D_CALL(dev->native->CreateBlendState(&desc_dx11, &native));

        BlendStateDX11 res;
        res.native = native;

        return dev->resources.blend_states.push(res);
    }

    void Gfx_BlendState_Destroy(RenderDevice* dev, BlendStateHandle h)
    {
        if( h.valid() )
        {
            SafeRelease(dev->resources.blend_states[h].native);
            dev->resources.blend_states.remove(h);
        }
    }

    // sampler state
    SamplerStateHandle Gfx_SamplerState_Create(RenderDevice* dev, const SamplerStateDescr& descr)
    {
        (void)descr; // TODO: convert state correctly

        D3D11_SAMPLER_DESC desc_dx11;
        
        desc_dx11.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
        desc_dx11.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
        desc_dx11.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
        desc_dx11.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
        desc_dx11.MipLODBias = 0;
        desc_dx11.MaxAnisotropy = 1;
        desc_dx11.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
        desc_dx11.BorderColor[0] = 0;
        desc_dx11.BorderColor[1] = 0;
        desc_dx11.BorderColor[2] = 0;
        desc_dx11.BorderColor[3] = 0;
        desc_dx11.MinLOD = 0;
        desc_dx11.MaxLOD = D3D11_FLOAT32_MAX;

        ID3D11SamplerState* native = NULL;
        D3D_CALL(dev->native->CreateSamplerState(&desc_dx11, &native));

        SamplerStateDX11 res;

        res.native = native;

        return dev->resources.sampler_states.push(res);
    }

    void Gfx_SamplerState_Destroy(RenderDevice* dev, SamplerStateHandle h)
    {
        if( h.valid() )
        {
            SafeRelease(dev->resources.sampler_states[h].native);
            dev->resources.sampler_states.remove(h);
        }
    }

    // render target
    TextureHandle Gfx_RenderTarget_Create(RenderDevice* dev, const TextureDescr& tex)
    {
        (void)dev;
        (void)tex;
        // TODO
        return InvalidResourceHandle();
    }

    // vertex buffer
    VertexBufferHandle Gfx_VertexBuffer_Create(RenderDevice* dev, const VertexBufferDescr& vb, const void* data)
    {
        D3D11_BUFFER_DESC desc;

        desc.ByteWidth = vb.size;
        desc.Usage = dx11_convert<D3D11_USAGE>(vb.mode);
        desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        desc.CPUAccessFlags = dx11_convert<D3D11_CPU_ACCESS_FLAG>(vb.mode);
        desc.MiscFlags = 0;
        desc.StructureByteStride = vb.stride;

        D3D11_SUBRESOURCE_DATA initial_data;
        initial_data.pSysMem = data;
        initial_data.SysMemPitch = 0;
        initial_data.SysMemSlicePitch = 0;

        ID3D11Buffer* native = NULL;

        D3D_CALL(dev->native->CreateBuffer(&desc, data?&initial_data:NULL, &native));

        VertexBufferDX11 res;

        res.desc = vb;
        res.native = native;

        return dev->resources.vertex_buffers.push(res);
    }

    VertexBufferLock Gfx_VertexBuffer_Lock(RenderDevice* dev, VertexBufferHandle h, uint32 offset, uint32 size)
    { 
        VertexBufferLock lock;

        if( h.valid() )
        {
            lock.size = size;
            lock.handle = h;

            D3D11_MAP type = dx11_convert<D3D11_MAP>(dev->resources.vertex_buffers[h].desc.mode);
            uint32 flags = 0;

            ID3D11Buffer* native = dev->resources.vertex_buffers[h].native;

            D3D11_MAPPED_SUBRESOURCE res;
            D3D_CALL(dev->default_context->native->Map(native, 0, type, flags, &res));

            lock.data = (char*)res.pData + offset;
        }

        return lock;
    }
    
    void Gfx_VertexBuffer_Unlock(RenderDevice* dev, VertexBufferLock& lock)
    {
        if( lock.handle.valid() )
        {
            ID3D11Buffer* native = dev->resources.vertex_buffers[lock.handle].native;
            dev->default_context->native->Unmap(native, 0);
            lock.data = NULL;
            lock.size = 0;
        }
    }

    void Gfx_VertexBuffer_Update(RenderDevice* dev, VertexBufferHandle h, void* data, uint32 offset, uint32 size, bool discard)
    {
        (void)discard; // TODO

        size = size ? size : dev->resources.vertex_buffers[h].desc.size;		
        VertexBufferLock lock = Gfx_VertexBuffer_Lock(dev, h, offset, size);
        memcpy(lock.data, data, size);
        Gfx_VertexBuffer_Unlock(dev, lock);
    }

    void Gfx_VertexBuffer_Destroy(RenderDevice* dev, VertexBufferHandle vb)
    {
        if( vb.valid() )
        {
            dev->resources.vertex_buffers[vb].native->Release();
            dev->resources.vertex_buffers.remove(vb);
        }
    }

    // index buffer
    IndexBufferHandle Gfx_IndexBuffer_Create(RenderDevice* dev, const IndexBufferDescr& ib, const void* data)
    {
        D3D11_BUFFER_DESC desc;

        desc.ByteWidth = ib.size;
        desc.Usage = dx11_convert<D3D11_USAGE>(ib.mode);
        desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
        desc.CPUAccessFlags = dx11_convert<D3D11_CPU_ACCESS_FLAG>(ib.mode);
        desc.MiscFlags = 0;
        desc.StructureByteStride = ib.stride;

        D3D11_SUBRESOURCE_DATA initial_data;
        initial_data.pSysMem = data;
        initial_data.SysMemPitch = 0;
        initial_data.SysMemSlicePitch = 0;

        ID3D11Buffer* native = NULL;

        D3D_CALL(dev->native->CreateBuffer(&desc, data?&initial_data:NULL, &native));

        IndexBufferDX11 res;

        res.desc = ib;
        res.native = native;
        res.native_format = ib.stride == 2 ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT;

        return dev->resources.index_buffers.push(res);
    }

    IndexBufferLock Gfx_IndexBuffer_Lock(RenderDevice* dev, IndexBufferHandle h, uint32 offset, uint32 size) 
    { 
        IndexBufferLock lock;

        if( h.valid() )
        {
            lock.size = size;
            lock.handle = h;

            D3D11_MAP type = dx11_convert<D3D11_MAP>(dev->resources.index_buffers[h].desc.mode);
            uint32 flags = 0;

            ID3D11Buffer* native = dev->resources.index_buffers[h].native;

            D3D11_MAPPED_SUBRESOURCE res;
            D3D_CALL(dev->default_context->native->Map(native, 0, type, flags, &res));

            lock.data = (char*)res.pData + offset;
        }

        return lock;
    }

    void Gfx_IndexBuffer_Unlock(RenderDevice* dev, IndexBufferLock& lock)
    {
        if( lock.handle.valid() )
        {
            ID3D11Buffer* native = dev->resources.index_buffers[lock.handle].native;
            dev->default_context->native->Unmap(native, 0);
            lock.data = NULL;
            lock.size = 0;
        }
    }

    void Gfx_IndexBuffer_Update(RenderDevice* dev, IndexBufferHandle h, void* data, uint32 offset, uint32 size, bool discard)
    {
        (void)discard; // TODO

        size = size ? size : dev->resources.index_buffers[h].desc.size;
        IndexBufferLock lock = Gfx_IndexBuffer_Lock(dev, h, offset, size);
        memcpy(lock.data, data, size);
        Gfx_IndexBuffer_Unlock(dev, lock);
    }

    void Gfx_IndexBuffer_Destroy(RenderDevice* dev, IndexBufferHandle ib)
    {
        if( ib.valid() )
        {
            dev->resources.index_buffers[ib].native->Release();
            dev->resources.index_buffers.remove(ib);
        }
    }

    // context
    RenderContext* Gfx_Immediate_Context(RenderDevice* dev) 
    { 
        return dev->default_context; 
    }

    void Gfx_Context_SetScissorRect(RenderContext* rc, const Rect& rect)
    {
        D3D11_RECT native_rect;

        native_rect.left = rect.left;
        native_rect.top = rect.top;
        native_rect.right = rect.right;
        native_rect.bottom = rect.bottom;

        rc->native->RSSetScissorRects(1, &native_rect);
    }

    void Gfx_Context_SetViewport(RenderContext* rc, const Viewport& next)
    {
        D3D11_VIEWPORT native_vp;

        native_vp.TopLeftX = next.x;
        native_vp.TopLeftY = next.y;
        native_vp.Width = next.w;
        native_vp.Height = next.h;
        native_vp.MinDepth = next.depth_min;
        native_vp.MaxDepth = next.depth_max;

        rc->native->RSSetViewports(1, &native_vp);
    }

    void Gfx_Context_SetTechnique(RenderContext* rc, TechniqueHandle h)
    {
         const TechniqueDX11& t = rc->resources->techniques[h];
         rc->native->IASetInputLayout(t.input_layout);
         rc->native->PSSetShader(rc->resources->pixel_shaders[t.ps].native, NULL, 0);
         rc->native->VSSetShader(rc->resources->vertex_shaders[t.vs].native, NULL, 0);

         // set constants in a stupid way, emulating GLES2 behavior

         Vector4 ps_constant_data[MaxShaderConstants];
         Vector4 vs_constant_data[MaxShaderConstants];

         int32 psc_dirty_first = MaxShaderConstants;
         int32 psc_dirty_last  = 0;

         for( uint32 i=0; i<t.ps_bindings.size(); ++i )
         {
             int32 first = (int32)t.ps_bindings[i].index;
             int32 count = (int32)t.ps_bindings[i].count;
             int32 last  = first+count;
             rush_memcpy(&ps_constant_data[first], (const float*)t.ps_bindings[i].data, sizeof(Vector4)*count);
             psc_dirty_first = rush_min(psc_dirty_first, first);
             psc_dirty_last  = rush_max(psc_dirty_last,  last);
         }

         int32 vsc_dirty_first = MaxShaderConstants;
         int32 vsc_dirty_last  = 0;

         for( uint32 i=0; i<t.vs_bindings.size(); ++i )
         {
             int32 first = (int32)t.vs_bindings[i].index;
             int32 count = (int32)t.vs_bindings[i].count;
             int32 last  = first+count;
             rush_memcpy(&vs_constant_data[first], (const float*)t.vs_bindings[i].data, sizeof(Vector4)*count);
             vsc_dirty_first = rush_min(vsc_dirty_first, first);
             vsc_dirty_last  = rush_max(vsc_dirty_last,  last);
         }

         if( psc_dirty_first!=MaxShaderConstants )
         {
             D3D11_MAPPED_SUBRESOURCE mr;
             D3D_CALL(rc->native->Map(rc->ps_cb, 0, D3D11_MAP_WRITE_DISCARD, 0, &mr));
             Vector4* data_v4f = (Vector4*)mr.pData;
             rush_memcpy(&data_v4f[psc_dirty_first], &ps_constant_data[psc_dirty_first], sizeof(Vector4)*(psc_dirty_last-psc_dirty_first));
             rc->native->Unmap(rc->ps_cb, 0);
             rc->native->PSSetConstantBuffers(0, 1, &rc->ps_cb);
         }

         if( vsc_dirty_first!=MaxShaderConstants )
         {
             D3D11_MAPPED_SUBRESOURCE mr;
             D3D_CALL(rc->native->Map(rc->vs_cb, 0, D3D11_MAP_WRITE_DISCARD, 0, &mr));
             Vector4* data_v4f = (Vector4*)mr.pData;
             rush_memcpy(&data_v4f[vsc_dirty_first], &vs_constant_data[vsc_dirty_first], sizeof(Vector4)*(vsc_dirty_last-vsc_dirty_first));
             rc->native->Unmap(rc->vs_cb, 0);
             rc->native->VSSetConstantBuffers(0, 1, &rc->vs_cb);
         }

    }

    void Gfx_Context_SetPrimitive(RenderContext* rc, PrimitiveType type)
    {
        rc->native->IASetPrimitiveTopology(dx11_convert<D3D11_PRIMITIVE_TOPOLOGY>(type));
    }

    void Gfx_Context_SetIndexStream(RenderContext* rc, IndexBufferHandle h)
    {
        if( h.valid() )
        {
            const IndexBufferDX11& ib = rc->resources->index_buffers[h];
            rc->native->IASetIndexBuffer(ib.native, ib.native_format, 0);
        }
        else
        {
            rc->native->IASetIndexBuffer(NULL, DXGI_FORMAT_R32_UINT, 0);
        }
    }

    void Gfx_Context_SetVertexStream(RenderContext* rc, uint32 idx, VertexBufferHandle h)
    {
        const VertexBufferDX11& vb = rc->resources->vertex_buffers[h];

        ID3D11Buffer* buffer = vb.native;
        uint32 stride = vb.desc.stride;
        uint32 offset = 0;

        rc->native->IASetVertexBuffers(idx, 1, &buffer, &stride, &offset);
    }

    void Gfx_Context_SetPixelTexture(RenderContext* rc, uint32 idx, TextureHandle h)
    {
        const TextureDX11& t = rc->resources->textures[h];
        rc->native->PSSetShaderResources(idx, 1, &t.srv);
    }
    
    void Gfx_Context_SetPixelSamplerState(RenderContext* rc, uint32 idx, SamplerStateHandle h)
    {
        const SamplerStateDX11& s = rc->resources->sampler_states[h];
        rc->native->PSSetSamplers(idx, 1, &s.native);
    }

    void Gfx_Context_SetBlendState(RenderContext* rc, BlendStateHandle h)
    {
        const float blend_factor[4] = {0.0f, 0.0f, 0.0f, 0.0f};
        uint32 sample_mask = 0xFFFFFFFF;
        rc->native->OMSetBlendState(rc->resources->blend_states[h].native, blend_factor, sample_mask);
    }

    void Gfx_Context_Draw(RenderContext* rc, uint32 first_vertex, uint32 vertex_count)
    {
        rc->native->Draw(vertex_count, first_vertex);
    }

    void Gfx_Context_DrawIndexed(RenderContext* rc, uint32 index_count, uint32 first_index, uint32 base_vertex, uint32 /*num_vertices*/)
    {
        rc->native->DrawIndexed(index_count, first_index, base_vertex);
    }

    DepthStencilStateHandle Gfx_DepthStencilState_Create(RenderDevice* dev, const DepthStencilStateDescr& descr)
    {
        // TODO

        D3D11_DEPTH_STENCIL_DESC desc_dx11;

        desc_dx11.DepthEnable = descr.enable;
        desc_dx11.DepthWriteMask = descr.write_enable ? D3D11_DEPTH_WRITE_MASK_ALL : D3D11_DEPTH_WRITE_MASK_ZERO;
        desc_dx11.DepthFunc = dx11_convert<D3D11_COMPARISON_FUNC>(descr.cmp_func);
        desc_dx11.StencilEnable = FALSE;
        desc_dx11.StencilReadMask = 0xFF;
        desc_dx11.StencilWriteMask = 0xFF;
        desc_dx11.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
        desc_dx11.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
        desc_dx11.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
        desc_dx11.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
        desc_dx11.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
        desc_dx11.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
        desc_dx11.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
        desc_dx11.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

        ID3D11DepthStencilState* state_dx11 = NULL;
        D3D_CALL(dev->native->CreateDepthStencilState(&desc_dx11, &state_dx11));

        return dev->resources.depth_stencil_states.push(state_dx11);
    }

    void Gfx_DepthStencilState_Destroy(RenderDevice* dev, DepthStencilStateHandle h)
    {
        if(h.valid())
        {
            SafeRelease(dev->resources.depth_stencil_states[h]);
            dev->resources.depth_stencil_states.remove(h);
        }
    }

    void Gfx_Context_SetDepthStencilState(RenderContext* rc, DepthStencilStateHandle next_state)
    {
        rc->native->OMSetDepthStencilState(rc->resources->depth_stencil_states[next_state], 0);
    }

}


#else //RUSH_RENDER_API==RUSH_RENDER_API_DX11

char _DX11RenderDevice_cpp; // suppress linker warning

#endif //RUSH_RENDER_API==RUSH_RENDER_API_DX11
