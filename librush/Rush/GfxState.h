#pragma once

#include "Rush.h"

namespace Rush
{
    struct Blend
    {		
        enum type 
        {
            Zero,
            One,
            SrcColour,
            InvSrcColour,
            SrcAlpha,
            InvSrcAlpha,
            DestAlpha,
            InvDestAlpha,
            DestColour,
            InvDestColour,

            _num_elements
        };
    };

    struct BlendOp
    {
        enum type 
        {
            Add,
            Subtract,
            RevSubtract,
            Min,
            Max,

            _num_elements
        };
    };

    struct TextureFilter
    {
        enum type 
        {
            Point,
            Linear,
            Anisotropic,
            _num_elements
        };
    };

    struct TextureWrap
    {
        enum type 
        {
            Wrap,
            Mirror,
            Clamp,
            Borderk,
            _num_elements
        };
    };

    struct CompareFunc
    {
        enum type 
        {
            Never,
            Less,
            Equal,
            LessEqual,
            Greater,
            NotEqual,
            GreaterEqual,
            Always,
            _num_elements
        };
    };

    struct BlendStateDescr
    {
        Blend::type		src;
        Blend::type		dst;
        BlendOp::type	op;

        Blend::type		alpha_src;
        Blend::type		alpha_dst;
        BlendOp::type	alpha_op;

        bool			alpha_separate;
        bool			enable;

        // Helper functions to create common blend states

        static BlendStateDescr make_solid();
        static BlendStateDescr make_alphablend();
        static BlendStateDescr make_additive();
        static BlendStateDescr make_premultiplied_alpha();

    };

    struct SamplerStateDescr
    {
        TextureFilter::type filter_min;
        TextureFilter::type filter_mag;
        TextureFilter::type filter_mip;

        // Helper functions

        static SamplerStateDescr make_linear();
        static SamplerStateDescr make_point();
    };

    struct DepthStencilStateDescr
    {
        CompareFunc::type cmp_func;
        bool enable;
        bool write_enable;
        

        //TODO: Stencil and others
    };

}
