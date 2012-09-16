#include "GfxState.h"

namespace Rush
{
	BlendStateDescr BlendStateDescr::make_solid()
	{
		BlendStateDescr res;

		res.enable         = false;
		res.src            = Blend::One;
		res.dst            = Blend::Zero;
		res.op	           = BlendOp::Add;
		res.alpha_separate = false;
		res.alpha_src      = Blend::One;
		res.alpha_dst      = Blend::Zero;
		res.alpha_op       = BlendOp::Add;
		
		return res;
	}

	BlendStateDescr BlendStateDescr::make_alphablend()
	{
		BlendStateDescr res;

		res.enable         = true;
		res.src            = Blend::SrcAlpha;
		res.dst            = Blend::InvSrcAlpha;
		res.op	           = BlendOp::Add;
		res.alpha_separate = false;
		res.alpha_src      = Blend::SrcAlpha;
		res.alpha_dst      = Blend::InvSrcAlpha;
		res.alpha_op       = BlendOp::Add;

		return res;
	}

	BlendStateDescr BlendStateDescr::make_additive()
	{
		BlendStateDescr res;

		res.enable         = true;
		res.src            = Blend::One;
		res.dst            = Blend::One;
		res.op	           = BlendOp::Add;
		res.alpha_separate = false;
		res.alpha_src      = Blend::One;
		res.alpha_dst      = Blend::One;
		res.alpha_op       = BlendOp::Add;

		return res;
	}


	BlendStateDescr BlendStateDescr::make_premultiplied_alpha()
	{
		BlendStateDescr res;

		res.enable         = true;
		res.src            = Blend::One;
		res.dst            = Blend::InvSrcAlpha;
		res.op	           = BlendOp::Add;
		res.alpha_separate = true;
		res.alpha_src      = Blend::One;
		res.alpha_dst      = Blend::One;
		res.alpha_op       = BlendOp::Add;

		return res;
	}

	SamplerStateDescr SamplerStateDescr::make_linear()
	{
		SamplerStateDescr res;

		res.filter_min = TextureFilter::Linear;
		res.filter_mag = TextureFilter::Linear;
		res.filter_mip = TextureFilter::Linear;

		return res;
	}

	SamplerStateDescr SamplerStateDescr::make_point()
	{
		SamplerStateDescr res;

		res.filter_min = TextureFilter::Point;
		res.filter_mag = TextureFilter::Point;
		res.filter_mip = TextureFilter::Point;

		return res;
	}
}
