#pragma once

#include "GfxApi.h"
#include "GfxDevice.h"

#if RUSH_RENDER_API==RUSH_RENDER_API_DX11

#include "GfxTexture.h"
#include "GfxVertexFormat.h"
#include "GfxBuffer.h"
#include "GfxShader.h"

#include <D3D11.h>
#include <D3DX11.h>
#include <D3Dcompiler.h>
#include <DxErr.h>

namespace Rush
{
    struct TextureDX11
    {
        TextureDX11() 
            : native2D(NULL)
            , native3D(NULL)
            , nativeCUBE(NULL)
            , rtv(NULL) 
            , srv(NULL)
        {
        }

        TextureDescr desc;
        ID3D11Texture2D* native2D;
        ID3D11Texture3D* native3D;
        ID3D11Texture2D* nativeCUBE;
        ID3D11RenderTargetView* rtv;
        ID3D11ShaderResourceView* srv;
    };

    struct IndexBufferDX11
    {
        IndexBufferDX11()
            : native(NULL)
        {}
        IndexBufferDescr desc;
        ID3D11Buffer* native;
        DXGI_FORMAT native_format;
    };

    struct VertexBufferDX11
    {
        VertexBufferDX11()
            : native(NULL)
        {}
        VertexBufferDescr desc;
        ID3D11Buffer* native;
    };

    struct RenderTargetDX11
    {
        RenderTargetDX11()
            : tex(NULL)
            , rt_view(NULL)
            , ds_view(NULL)
        {}

        ID3D11Texture2D*        tex;
        ID3D11RenderTargetView* rt_view;
        ID3D11DepthStencilView* ds_view;
    };

    struct VertexShaderDX11
    {
        inline VertexShaderDX11() 
            : native(NULL) 
        {}

        ID3D11VertexShader* native;
        ShaderCode code;
        ShaderConstantTable constants;
        ShaderConstantTable samplers;
    };

    struct PixelShaderDX11
    {
        inline PixelShaderDX11() 
            : native(NULL) 
        {}

        ID3D11PixelShader* native;
        ShaderCode code;
        ShaderConstantTable constants;
        ShaderConstantTable samplers;
    };

    struct TechniqueDX11
    {
        struct Binding : public ShaderConstantIndex
        {
            const Vector4* data;
        };

        PixelShaderHandle       ps;
        DynamicArray<Binding>   ps_bindings;

        VertexShaderHandle      vs;
        DynamicArray<Binding>   vs_bindings;

        VertexFormatHandle      vf;
        ID3D11InputLayout*      input_layout;
    };

    struct VertexFormatDX11
    {
        VertexFormatDescr descr;
    };

    struct SamplerStateDX11
    {
        SamplerStateDX11() : native(NULL){}
        ID3D11SamplerState* native;
    };

    struct BlendStateDX11
    {
        BlendStateDX11() : native(NULL){}
        ID3D11BlendState* native;
    };

    enum { MaxShaderConstants=256 }; // TODO: find a better place for this

    class RenderResourcesDX11
    {
    public:

        ResourcePool<VertexFormatDX11,          VertexFormatHandle,         GfxMaxVertexFormats>        vertex_formats;
        ResourcePool<VertexShaderDX11,          VertexShaderHandle,         GfxMaxVertexShaders>        vertex_shaders;
        ResourcePool<PixelShaderDX11,           PixelShaderHandle,          GfxMaxPixelShaders>         pixel_shaders;
        ResourcePool<TextureDX11,               TextureHandle,              GfxMaxTextures>             textures;
        ResourcePool<IndexBufferDX11,           IndexBufferHandle,          GfxMaxIndexBuffers>         index_buffers;
        ResourcePool<VertexBufferDX11,          VertexBufferHandle,         GfxMaxVertexBuffers>        vertex_buffers;
        ResourcePool<SamplerStateDX11,          SamplerStateHandle,         GfxMaxSamplerStates>        sampler_states;
        ResourcePool<BlendStateDX11,            BlendStateHandle,           GfxMaxBlendStates>          blend_states;
        ResourcePool<TechniqueDX11,             TechniqueHandle,            GfxMaxTechniques>           techniques;
        ResourcePool<ID3D11DepthStencilState*,  DepthStencilStateHandle,    GfxMaxDepthStencilStates>   depth_stencil_states;
    };

    class RenderContext
    {
    public:

        RenderContext()
            : native(NULL)
            , resources(NULL)
            , current_rtv(NULL)
            , current_dsv(NULL)
            , ps_cb(NULL)
            , vs_cb(NULL)
        {
        }

        ID3D11DeviceContext* native;
        RenderResourcesDX11* resources;

        ID3D11RenderTargetView* current_rtv;
        ID3D11DepthStencilView* current_dsv;

        ID3D11Buffer* ps_cb;
        ID3D11Buffer* vs_cb;
    };

    class RenderDevice
    {
    public:

        RenderDevice()
            : native(NULL)
            , swap_chain(NULL)
            , default_context(NULL)
            , backbuffer_tex(NULL)
            , backbuffer_rtv(NULL)
            , depthstencil_tex(NULL)
            , depthstencil_dsv(NULL)
            , vsync(0)
        {
        }

        ID3D11Device*           native;
        IDXGISwapChain*         swap_chain;
        RenderContext*          default_context;

        ID3D11Texture2D*        backbuffer_tex;
        ID3D11RenderTargetView* backbuffer_rtv;

        ID3D11Texture2D*        depthstencil_tex;
        ID3D11DepthStencilView* depthstencil_dsv;

        RenderResourcesDX11     resources;

        ReferenceCount          refcount;

        uint32                  vsync;
    };

}

#endif //RUSH_RENDER_API==RUSH_RENDER_API_DX11
