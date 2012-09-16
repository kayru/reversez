
#if OLD_INTERFACE

    //////////////////////////////////////////////////////////////////////////

    RenderContextDX9::RenderContextDX9()
        : m_device(NULL)
    {
    }

    void RenderContextDX9::dx9_init( IDirect3DDevice9* device, const RenderResourcesDX9* resources, RenderDeviceStats* stats )
    {
        m_device = device;
        m_resources = resources;    
        m_primitive_type = D3DPT_TRIANGLELIST;
        m_stats = stats;
    }

    void RenderContextDX9::commit()
    {
        //////////////////////////////////////////////////////////////////////////

        if( technique.is_dirty() )
        {
            const TechniqueDX9& t = m_resources->techniques[technique.next().index()];

            vertex_format.set_next(t.vf);
            vertex_shader.set_next(t.vs);
            pixel_shader.set_next(t.ps);

            technique.commit();
        }

        if( technique.curr().valid() )
        {
            const TechniqueDX9& t = m_resources->techniques[technique.curr().index()];

            for( uint32 i=0; i<t.vs_bindings.size(); ++i )
            {
                m_device->SetVertexShaderConstantF(t.vs_bindings[i].index, (const float*)t.vs_bindings[i].data, t.vs_bindings[i].count);
            }

            for( uint32 i=0; i<t.ps_bindings.size(); ++i )
            {
                m_device->SetPixelShaderConstantF(t.ps_bindings[i].index, (const float*)t.ps_bindings[i].data, t.ps_bindings[i].count);
            }
        }

        //////////////////////////////////////////////////////////////////////////

        if( index_stream.is_dirty() )
        {
            if( index_stream.next().valid() )
            {
                const IndexBufferDX9& ib = m_resources->index_buffers[index_stream.next().index()];
                m_device->SetIndices(ib.native);
            }
            else
            {
                m_device->SetIndices(NULL);
            }
            index_stream.commit();
        }

        if( vertex_streams.is_dirty() )
        {
            uint32 vs_first = vertex_streams.dirty_first();
            uint32 vs_last  = vertex_streams.dirty_last();
            for( uint32 i=vs_first; i<vs_last; ++i )
            {
                if( vertex_streams.next(i).valid() )
                {
                    const VertexBufferDX9& vb = m_resources->vertex_buffers[vertex_streams.next(i).index()];
                    m_device->SetStreamSource(i, vb.native, 0, vb.desc.stride);
                }
                else
                {
                    m_device->SetStreamSource(i, NULL, 0, 4);
                }
            }
            vertex_streams.commit();
        }

        if( primitive_type.is_dirty() )
        {
            m_primitive_type = (uint32)primitive_type_to_dx9(primitive_type.next());
            primitive_type.commit();
        }

        //////////////////////////////////////////////////////////////////////////

        if( vertex_format.is_dirty() )
        {
            if( vertex_format.next().valid() )
            {
                const VertexFormatDX9& vf = m_resources->vertex_formats[vertex_format.next().index()];
                m_device->SetVertexDeclaration(vf.native);
            }
            else
            {
                m_device->SetVertexDeclaration(NULL);
            }
            vertex_format.commit();
        }

        //////////////////////////////////////////////////////////////////////////



        //////////////////////////////////////////////////////////////////////////

        if( pixel_shader.is_dirty() )
        {
            IDirect3DPixelShader9* sh = pixel_shader.next().valid() ? m_resources->pixel_shaders[pixel_shader.next().index()].native : NULL;
            m_device->SetPixelShader(sh);
            pixel_shader.commit();
        }

        if( vertex_shader.is_dirty() )
        {
            IDirect3DVertexShader9* sh = vertex_shader.next().valid() ? m_resources->vertex_shaders[vertex_shader.next().index()].native : NULL;
            m_device->SetVertexShader(sh);
            vertex_shader.commit();
        }

        if( ps_textures.is_dirty() )
        {
            uint32 first = ps_textures.dirty_first();
            uint32 last  = ps_textures.dirty_last();
            for( uint32 idx=first; idx<last; ++idx )
            {
                IDirect3DBaseTexture9* tex_dx9 = NULL; 
                TextureHandle h = ps_textures.next(idx);
                if( h.valid() )
                {
                    const TextureDX9& tex = m_resources->textures[h.index()];
                    switch(m_resources->textures[h.index()].desc.type)
                    {
                    case TextureType::Tex2D:
                        tex_dx9 = tex.native2D;
                        break;
                    case TextureType::Tex3D:
                        tex_dx9 = tex.native3D;
                        break;
                    case TextureType::TexCUBE:
                        tex_dx9 = tex.nativeCUBE;
                        break;
                    }
                }
                m_device->SetTexture(idx, tex_dx9);
            }
            ps_textures.commit();
        }
    }

    //////////////////////////////////////////////////////////////////////////

    void RenderDeviceDX9::pr_destroy_device()
    {
        SafeRelease(m_default_backbuffer);
        SafeRelease(m_default_depthbuffer);
        m_resources->release();
        SafeRelease(m_device);
    }

    TextureHandle RenderDeviceDX9::create_render_target( const TextureDescr& tex )
    {
        D3DFORMAT format_dx9 = pixel_format_to_dx9(tex.format);

        uint32 usage = D3DUSAGE_RENDERTARGET;
        D3DPOOL pool = D3DPOOL_DEFAULT;

        IDirect3DTexture9* tex_dx9 = NULL;
        IDirect3DSurface9* surf_dx9 = NULL;

        HRESULT hr;

        D3DFORMAT min_depth_stencil_fmt = D3DFMT_D16_LOCKABLE;
        D3DFORMAT max_depth_stencil_fmt = D3DFMT_S8_LOCKABLE;

        if( format_dx9>=min_depth_stencil_fmt && format_dx9<=max_depth_stencil_fmt)
        {
            usage = D3DUSAGE_DEPTHSTENCIL;
        }
        
        hr = m_device->CreateTexture(tex.width, tex.height, 1, usage, format_dx9, pool, &tex_dx9, NULL);
        validate_d3d_result(hr);

        hr = tex_dx9->GetSurfaceLevel(0, &surf_dx9);
        validate_d3d_result(hr);

        TextureDX9 res;
        res.desc = tex;
        res.native2D = tex_dx9;
        res.surface = surf_dx9;
        res.desc.render_target = true;

        return m_resources->textures.push(res);
    }


#endif //0

