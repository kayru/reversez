#pragma once

#include "GfxApi.h"

#include "MathAABB2.h"
#include "UtilResourcePool.h"

namespace Rush
{	
	class Window;
	class RenderDevice;
	class RenderContext;

	class VertexFormatDescr;
	class VertexShaderDescr;
	class PixelShaderDescr;
	struct TextureDescr;
	struct IndexBufferDescr;
	struct VertexBufferDescr;
	struct SamplerStateDescr;
	struct BlendStateDescr;
	struct TechniqueDescr;
    struct DepthStencilStateDescr;

	typedef ResourceHandle<VertexFormatDescr>       VertexFormatHandle;
	typedef ResourceHandle<VertexShaderDescr>       VertexShaderHandle;
	typedef ResourceHandle<PixelShaderDescr>        PixelShaderHandle;
	typedef ResourceHandle<TextureDescr>            TextureHandle;
	typedef ResourceHandle<IndexBufferDescr>        IndexBufferHandle;
	typedef ResourceHandle<VertexBufferDescr>       VertexBufferHandle;
	typedef ResourceHandle<SamplerStateDescr>       SamplerStateHandle;
	typedef ResourceHandle<BlendStateDescr>         BlendStateHandle;
    typedef ResourceHandle<DepthStencilStateDescr>  DepthStencilStateHandle;
	typedef ResourceHandle<TechniqueDescr>          TechniqueHandle;

	struct PrimitiveTypes
	{
		enum PrimitiveType
		{
			PointList,
			LineList,
			LineStrip,
			TriangleList,
			TriangleStrip,

			_num_elements
		};
	};
	typedef PrimitiveTypes::PrimitiveType PrimitiveType;

	struct ClearFlags
	{
		enum type 
		{
			Colour	= 1<<0,
			Depth	= 1<<1,
			Stencil	= 1<<2,

			Default = Colour|Depth|Stencil
		};
	};

	template <typename T> 
	class StateCache
	{
	public:

		inline StateCache(const T& v)
			: m_curr(v)
			, m_next(v)
			, m_dirty(false)
		{
		}

		inline void init(const T& v) { m_curr = m_next = v; m_dirty = false; }
		inline void set_next(const T& v) { m_next = v; m_dirty = m_curr!=m_next;}
		inline bool is_dirty() { return m_dirty; }
		inline void commit() { m_curr=m_next; m_dirty = false; }

		inline const T& curr() { return m_curr; }
		inline const T& next() { return m_next; }

	private:

		T m_curr;
		T m_next;
		bool m_dirty;

	};

	template <typename T, size_t COUNT> 
	class StateArrayCache
	{

	public:

		inline StateArrayCache(const T& v){ init(v); }

		inline void init(const T& v)
		{
			for(size_t i=0; i<COUNT; ++i)
			{
				m_curr[i] = m_next[i] = v;
			}
			m_dirty_first = COUNT;
			m_dirty_last = 0;
			m_dirty = false;
		}

		inline bool is_dirty() 
		{
			return m_dirty;
		}

		inline void set_next(size_t idx, const T& v)
		{
			if( idx < COUNT )
			{
				m_next[idx] = v;
				if( curr(idx)!=next(idx) )
				{
					if( idx < m_dirty_first ) m_dirty_first = idx;
					if( idx >= m_dirty_last ) m_dirty_last = idx+1;
				}
				m_dirty = m_dirty_first < m_dirty_last;
			}
		}

		inline void set_next(size_t idx, const T* values, size_t count)
		{
			for( size_t i=0; i<count; ++i )
			{
				set_next(idx+i, values[i]);
			}
		}

		inline void commit()
		{
			m_dirty_last = m_dirty_last < COUNT ? m_dirty_last : COUNT;

			for(size_t i=m_dirty_first; i<m_dirty_last; ++i)
			{
				m_curr[i] = m_next[i];
			}

			m_dirty_first = COUNT;
			m_dirty_last = 0;
			m_dirty = false;
		}

		inline const T& curr(size_t i) const { return m_curr[i]; }
		inline const T& next(size_t i) const { return m_next[i]; }

		inline size_t dirty_first() const { return m_dirty_first; }
		inline size_t dirty_last() const { return m_dirty_last; }

	private:

		T m_curr[COUNT];
		T m_next[COUNT];

		size_t m_dirty_first;
		size_t m_dirty_last;
		bool m_dirty;

	};

	struct Viewport // TODO: move to Viewport.h
	{
		Viewport() 
			: x(0), y(0), w(1), h(1)
			, depth_min(0), depth_max(1)
		{
		}

		Viewport(const AABB2& bounds, float _depth_min=0, float _depth_max=1)
			: x(bounds.tl().x), y(bounds.tl().y), w(bounds.width()), h(bounds.height())
			, depth_min(_depth_min), depth_max(_depth_max)
		{
		}

		Viewport(float _x, float _y, float _w, float _h, float _depth_min=0, float _depth_max=1)
			: x(_x), y(_y), w(_w), h(_h)
			, depth_min(_depth_min), depth_max(_depth_max)
		{
		}

		bool operator == (const Viewport& rhs) const 
		{
			return 
				x == rhs.x && y == rhs.y &&
				w == rhs.w && h == rhs.h &&
				depth_min == rhs.depth_min &&
				depth_max == rhs.depth_max;
		}

		bool operator != (const Viewport& rhs) const 
		{
			return !(*this == rhs);
		}

		float x; // top left x
		float y; // top left y
		float w; // width
		float h; // height
		float depth_min;
		float depth_max;
	};

	struct Rect
	{
		int32 left, top, right, bottom;
	};

}
