#pragma once 

#include "GfxCommon.h"
#include "GfxState.h"
#include "GfxTexture.h"

#include "UtilReferenceCount.h"

#include "MathPixel.h"

namespace Rush
{
    struct ShaderCode;
    class RenderContext;
    class RenderDevice;

    enum
    {
        GfxMaxVertexFormats         = 1024,
        GfxMaxVertexShaders         = 1024,
        GfxMaxPixelShaders          = 1024,
        GfxMaxTextures              = 1024,
        GfxMaxIndexBuffers          = 1024,
        GfxMaxVertexBuffers         = 1024,
        GfxMaxSamplerStates         = 1024,
        GfxMaxBlendStates           = 1024,
        GfxMaxTechniques            = 1024,
        GfxMaxDepthStencilStates    = 1024,
    };

    struct RenderDeviceStats
    {
        uint32 draw_calls;
        uint32 vertices;
    };

    struct IndexBufferLock
    {
        inline IndexBufferLock():data(NULL),size(0),handle(){}
        void*  data;
        uint32 size;
        IndexBufferHandle handle;
    };

    struct VertexBufferLock
    {
        inline VertexBufferLock():data(NULL),size(0),handle(){}
        void*  data;
        uint32 size;
        VertexBufferHandle handle;
    };

    struct TextureLock
    {
        inline TextureLock():data(NULL),size(0),handle(){}
        void*  data;
        uint32 size;
        TextureHandle handle;
    };

    struct RenderTargetConfig
    {
        RenderTargetConfig()
            : c0(), c1(), c2(), c3(), ds()
        {}

        RenderTargetConfig(TextureHandle _c0, TextureHandle _c1, TextureHandle _c2, TextureHandle _c3, TextureHandle _ds)
            : c0(_c0), c1(_c1), c2(_c2), c3(_c3), ds(_ds)
        {}

        inline bool operator == (const RenderTargetConfig& rhs) const
        {
            return c0==rhs.c0 && c1==rhs.c1 && c2==rhs.c2 && c3==rhs.c3 && ds==rhs.ds;
        }

        inline bool operator != (const RenderTargetConfig& rhs) const { return !(*this==rhs); }

        TextureHandle c0;
        TextureHandle c1;
        TextureHandle c2;
        TextureHandle c3;
        TextureHandle ds;
    };

    struct RenderDeviceConfig
    {
        RenderDeviceConfig()
            : back_buffer_width(640)
            , back_buffer_height(480)
            , use_full_screen(false)
            , use_vertical_sync(false)
            , use_nvperfhud(false)
        {
        }
        uint32  back_buffer_width;
        uint32  back_buffer_height;
        bool    use_full_screen;
        bool    use_vertical_sync;
        bool    use_nvperfhud;
    };
   
    // various
    RenderDevice*               Gfx_CreateDevice                    (Window* window, const RenderDeviceConfig& cfg);
    void                        Gfx_DestroyDevice                   (RenderDevice* dev);
    void                        Gfx_AddReference                    (RenderDevice* dev);
    void                        Gfx_RemoveReference                 (RenderDevice* dev);
    void                        Gfx_FrameBegin                      (RenderDevice* dev);
    void                        Gfx_FrameEnd                        (RenderDevice* dev);
    void                        Gfx_Clear                           (RenderContext* rc, ColourRGBA8 colour, ClearFlags::type clear_flags = ClearFlags::Default, float depth=1.0f, uint32 stencil=0);
    void                        Gfx_Present                         (RenderDevice* dev);

    const RenderDeviceStats&    Gfx_Stats                           (RenderDevice* dev);
    void                        Gfx_ResetStats                      (RenderDevice* dev);

    // vertex format
    VertexFormatHandle          Gfx_VertexFormat_Create             (RenderDevice* dev, const VertexFormatDescr& fmt);
    void                        Gfx_VertexFormat_Destroy            (RenderDevice* dev, VertexFormatHandle h);

    // vertex shader
    VertexShaderHandle          Gfx_VertexShader_Create             (RenderDevice* dev, const ShaderCode& code);
    void                        Gfx_VertexShader_Destroy            (RenderDevice* dev, VertexShaderHandle& vsh);

    // pixel shader
    PixelShaderHandle           Gfx_PixelShader_Create              (RenderDevice* dev, const ShaderCode& code);
    void                        Gfx_PixelShader_Destroy             (RenderDevice* dev, PixelShaderHandle& psh);

    // technique
    TechniqueHandle             Gfx_Technique_Create                (RenderDevice* dev, const TechniqueDescr& descr);
    void                        Gfx_Technique_Destroy               (RenderDevice* dev, TechniqueHandle h);

    // texture
    TextureHandle               Gfx_Texture_Load                    (RenderDevice* dev, const char* filename, TextureType::type type=TextureType::Tex2D);
    TextureHandle               Gfx_Texture_Create                  (RenderDevice* dev, const TextureDescr& tex, const void* data=NULL, size_t size=0);
    const TextureDescr&         Gfx_Texture_Descr                   (RenderDevice* dev, TextureHandle h);
    void                        Gfx_Texture_Destroy                 (RenderDevice* dev, TextureHandle& th);

    // blend state
    BlendStateHandle            Gfx_BlendState_Create               (RenderDevice* dev, const BlendStateDescr& descr);
    void                        Gfx_BlendState_Destroy              (RenderDevice* dev, BlendStateHandle h);

    // sampler state
    SamplerStateHandle          Gfx_SamplerState_Create             (RenderDevice* dev, const SamplerStateDescr& descr);
    void                        Gfx_SamplerState_Destroy            (RenderDevice* dev, SamplerStateHandle h);

    // depth stencil state
    DepthStencilStateHandle     Gfx_DepthStencilState_Create        (RenderDevice* dev, const DepthStencilStateDescr& descr);
    void                        Gfx_DepthStencilState_Destroy       (RenderDevice* dev, DepthStencilStateHandle h);

    // render target
    TextureHandle               Gfx_RenderTarget_Create             (RenderDevice* dev, const TextureDescr& tex);

    // vertex buffer
    VertexBufferHandle          Gfx_VertexBuffer_Create             (RenderDevice* dev, const VertexBufferDescr& vb, const void* data=NULL);
    VertexBufferLock            Gfx_VertexBuffer_Lock               (RenderDevice* dev, VertexBufferHandle vb, uint32 offset, uint32 size);
    void                        Gfx_VertexBuffer_Unlock             (RenderDevice* dev, VertexBufferLock& lock);
    void                        Gfx_VertexBuffer_Update             (RenderDevice* dev, VertexBufferHandle h, void* data, uint32 offset=0, uint32 size=0, bool discard=false);
    void                        Gfx_VertexBuffer_Destroy            (RenderDevice* dev, VertexBufferHandle vb);    

    // index buffer
    IndexBufferHandle           Gfx_IndexBuffer_Create              (RenderDevice* dev, const IndexBufferDescr& ib, const void* data=NULL);
    IndexBufferLock             Gfx_IndexBuffer_Lock                (RenderDevice* dev, IndexBufferHandle ib, uint32 offset, uint32 size);
    void                        Gfx_IndexBuffer_Unlock              (RenderDevice* dev, IndexBufferLock& lock);
    void                        Gfx_IndexBuffer_Update              (RenderDevice* dev, IndexBufferHandle h, void* data, uint32 offset=0, uint32 size=0, bool discard=false);
    void                        Gfx_IndexBuffer_Destroy             (RenderDevice* dev, IndexBufferHandle ib);

    // context
    RenderContext*              Gfx_Immediate_Context               (RenderDevice* dev);

    void                        Gfx_Context_SetViewport             (RenderContext* rc, const Viewport& _viewport);
    void                        Gfx_Context_SetScissorRect          (RenderContext* rc, const Rect& rect);
    void                        Gfx_Context_SetTechnique            (RenderContext* rc, TechniqueHandle h);
    void                        Gfx_Context_SetPrimitive            (RenderContext* rc, PrimitiveType type);
    void                        Gfx_Context_SetIndexStream          (RenderContext* rc, IndexBufferHandle h);
    void                        Gfx_Context_SetVertexStream         (RenderContext* rc, uint32 idx, VertexBufferHandle h);
    void                        Gfx_Context_SetPixelTexture         (RenderContext* rc, uint32 idx, TextureHandle h);
    void                        Gfx_Context_SetPixelSamplerState    (RenderContext* rc, uint32 idx, SamplerStateHandle next_state);
    void                        Gfx_Context_SetBlendState           (RenderContext* rc, BlendStateHandle next_state);
    void                        Gfx_Context_SetDepthStencilState    (RenderContext* rc, DepthStencilStateHandle next_state);

    void                        Gfx_Context_Draw                    (RenderContext* rc, uint32 first_vertex, uint32 vertex_count);
    void                        Gfx_Context_DrawIndexed             (RenderContext* rc, uint32 index_count, uint32 first_index, uint32 base_vertex, uint32 num_vertices);

}

