#include "GfxDevice.h"

namespace Rush
{
#if 0
    // Null render interface implementation

    RenderDevice* Gfx_CreateDevice                    (Window* window, const RenderDeviceConfig& cfg) {return 0;}

    void Gfx_DestroyDevice(RenderDevice* dev){}
    void Gfx_AddReference(RenderDevice* dev){}
    void Gfx_RemoveReference(RenderDevice* dev){}
    void Gfx_FrameBegin(RenderDevice* dev){}
    void Gfx_FrameEnd(RenderDevice* dev){}
    void Gfx_Clear(RenderDevice* dev, ColourRGBA8 colour, ClearFlags::type clear_flags, float depth, uint32 stencil){}
    void Gfx_Present(RenderDevice* dev){}

    const RenderDeviceStats& Gfx_Stats(RenderDevice* dev){ static RenderDeviceStats s; return s; }
    void Gfx_ResetStats(RenderDevice* dev){}

    // vertex format
    VertexFormatHandle Gfx_VertexFormat_Create(RenderDevice* dev, const VertexFormatDescr& fmt) { return InvalidResourceHandle(); }
    void Gfx_VertexFormat_Destroy(RenderDevice* dev, VertexFormatHandle h);

    // vertex shader
    VertexShaderHandle Gfx_VertexShader_Create(RenderDevice* dev, const ShaderCode& code) { return InvalidResourceHandle(); }
    void Gfx_VertexShader_Destroy(RenderDevice* dev, VertexShaderHandle& vsh){};

    // pixel shader
    PixelShaderHandle Gfx_PixelShader_Create(RenderDevice* dev, const ShaderCode& code){ return InvalidResourceHandle(); }
    void Gfx_PixelShader_Destroy(RenderDevice* dev, PixelShaderHandle& psh){};

    // technique
    TechniqueHandle Gfx_Technique_Create(RenderDevice* dev, const TechniqueDescr& descr){ return InvalidResourceHandle(); }
    void Gfx_Technique_Destroy(RenderDevice* dev, TechniqueHandle h){};

    // texture
    TextureHandle Gfx_Texture_Load(RenderDevice* dev, const char* filename, TextureType::type type){ return InvalidResourceHandle(); }
    TextureHandle Gfx_Texture_Create(RenderDevice* dev, const TextureDescr& tex, const void* data, size_t size){ return InvalidResourceHandle(); }
    const TextureDescr& Gfx_Texture_Descr(RenderDevice* dev, TextureHandle h) { static TextureDescr s; return s; }
    void Gfx_Texture_Destroy(RenderDevice* dev, TextureHandle& th){};

    // blend state
    BlendStateHandle Gfx_BlendState_Create(RenderDevice* dev, const BlendStateDescr& descr){return InvalidResourceHandle();}
    void Gfx_BlendState_Destroy(RenderDevice* dev, BlendStateHandle h){};

    // sampler state
    SamplerStateHandle Gfx_SamplerState_Create(RenderDevice* dev, const SamplerStateDescr& descr){return InvalidResourceHandle();};
    void Gfx_SamplerState_Destroy(RenderDevice* dev, SamplerStateHandle h){};

    // render target
    TextureHandle Gfx_RenderTarget_Create(RenderDevice* dev, const TextureDescr& tex){return InvalidResourceHandle();};

    // vertex buffer
    VertexBufferHandle Gfx_VertexBuffer_Create(RenderDevice* dev, const VertexBufferDescr& vb, const void* data){return InvalidResourceHandle();};
    VertexBufferLock Gfx_VertexBuffer_Lock(RenderDevice* dev, VertexBufferHandle vb, uint32 offset, uint32 size) { static VertexBufferLock vl; return vl; }
    void Gfx_VertexBuffer_Unlock(RenderDevice* dev, VertexBufferLock& lock){};
    void Gfx_VertexBuffer_Update(RenderDevice* dev, VertexBufferHandle h, void* data, uint32 offset, uint32 size, bool discard){};
    void Gfx_VertexBuffer_Destroy(RenderDevice* dev, VertexBufferHandle vb){};

    // index buffer
    IndexBufferHandle Gfx_IndexBuffer_Create(RenderDevice* dev, const IndexBufferDescr& ib, const void* data){return InvalidResourceHandle();};
    IndexBufferLock Gfx_IndexBuffer_Lock(RenderDevice* dev, IndexBufferHandle ib, uint32 offset, uint32 size) { static IndexBufferLock il; return il; };
    void Gfx_IndexBuffer_Unlock(RenderDevice* dev, IndexBufferLock& lock){};
    void Gfx_IndexBuffer_Update(RenderDevice* dev, IndexBufferHandle h, void* data, uint32 offset, uint32 size, bool discard){};
    void Gfx_IndexBuffer_Destroy(RenderDevice* dev, IndexBufferHandle ib){};

    // context
    RenderContext* Gfx_Immediate_Context(RenderDevice* dev) { return NULL; }

    void Gfx_Context_SetViewport(RenderContext* rc, const Viewport& _viewport){};
    void Gfx_Context_SetTechnique(RenderContext* rc, TechniqueHandle h){};
    void Gfx_Context_SetPrimitive(RenderContext* rc, PrimitiveType type){};
    void Gfx_Context_SetIndexStream(RenderContext* rc, IndexBufferHandle h){};
    void Gfx_Context_SetVertexStream(RenderContext* rc, uint32 idx, VertexBufferHandle h){};
    void Gfx_Context_SetPixelTexture(RenderContext* rc, uint32 idx, TextureHandle h){};
    void Gfx_Context_SetPixelSamplerState(RenderContext* rc, uint32 idx, SamplerStateHandle next_state){};
    void Gfx_Context_SetBlendState(RenderContext* rc, BlendStateHandle next_state){};

    void Gfx_Context_Draw(RenderContext* rc, uint32 first_vertex, uint32 vertex_count){};
    void Gfx_Context_DrawIndexed(RenderContext* rc, uint32 index_count, uint32 first_index, uint32 base_vertex, uint32 num_vertices){};


#endif //1
}

