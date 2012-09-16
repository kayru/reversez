namespace Rush
{
	inline void PrimitiveBatch::draw_textured_quad( const AABB2& rect, TextureHandle tex, ColourRGBA8 colour )
	{
		draw_textured_quad(rect.bl(), rect.br(), rect.tr(), rect.tl(), tex, colour);
	}

	inline void PrimitiveBatch::draw_textured_quad( const TexturedQuad2D* q, TextureHandle tex, ColourRGBA8 colour )
	{
		draw_textured_quad(q->pos[0], q->pos[1], q->pos[2], q->pos[3], tex, colour, q->tex[0], q->tex[1], q->tex[2], q->tex[3]);
	}

	inline void PrimitiveBatch::draw_textured_quad(
		const Vector2& a, const Vector2& b, 
		const Vector2& c, const Vector2& d, 
		TextureHandle tex, ColourRGBA8 colour,
		const Vector2& ta, const Vector2& tb, 
		const Vector2& tc, const Vector2& td)
	{
		pr_start_new_batch(BatchType::TriList, tex);

		BatchVertex* v = m_vertices.alloc_unsafe(6);

		if( v && m_vertices.data() )
		{
			size_t i=0;

			v[i].pos.x = a.x;
			v[i].pos.y = a.y;
			v[i].pos.z = 0;
			v[i].tex   = ta;
			v[i].col0  = colour;
			++i;

			v[i].pos.x = b.x;
			v[i].pos.y = b.y;
			v[i].pos.z = 0;
			v[i].tex   = tb;
			v[i].col0  = colour;
			++i;

			v[i].pos.x = c.x;
			v[i].pos.y = c.y;
			v[i].pos.z = 0;
			v[i].tex   = tc;
			v[i].col0  = colour;
			++i;

			v[i].pos.x = a.x;
			v[i].pos.y = a.y;
			v[i].pos.z = 0;
			v[i].tex   = ta;
			v[i].col0  = colour;
			++i;

			v[i].pos.x = c.x;
			v[i].pos.y = c.y;
			v[i].pos.z = 0;
			v[i].tex   = tc;
			v[i].col0   = colour;
			++i;

			v[i].pos.x = d.x;
			v[i].pos.y = d.y;
			v[i].pos.z = 0;
			v[i].tex   = td;
			v[i].col0  = colour;
			++i;
		}
	}

	inline void PrimitiveBatch::draw_rect(const AABB2& rect, const ColourRGBA8& colour)  
	{
		pr_start_new_batch(BatchType::TriList);

		BatchVertex* v = m_vertices.alloc_unsafe(6);

		if( v && m_vertices.data() )
		{
			size_t i=0;

			v[i].pos.x = rect.tl().x;
			v[i].pos.y = rect.tl().y;
			v[i].pos.z = 0;
			v[i].tex.x = 0;
			v[i].tex.y = 0;
			v[i].col0   = colour;
			++i;

			v[i].pos.x = rect.bl().x;
			v[i].pos.y = rect.bl().y;
			v[i].pos.z = 0;
			v[i].tex.x = 0;
			v[i].tex.y = 0;
			v[i].col0   = colour;
			++i;

			v[i].pos.x = rect.br().x;
			v[i].pos.y = rect.br().y;
			v[i].pos.z = 0;
			v[i].tex.x = 0;
			v[i].tex.y = 0;
			v[i].col0   = colour;
			++i;

			v[i].pos.x = rect.tl().x;
			v[i].pos.y = rect.tl().y;
			v[i].pos.z = 0;
			v[i].tex.x = 0;
			v[i].tex.y = 0;
			v[i].col0   = colour;
			++i;

			v[i].pos.x = rect.br().x;
			v[i].pos.y = rect.br().y;
			v[i].pos.z = 0;
			v[i].tex.x = 0;
			v[i].tex.y = 0;
			v[i].col0   = colour;
			++i;

			v[i].pos.x = rect.tr().x;
			v[i].pos.y = rect.tr().y;
			v[i].pos.z = 0;
			v[i].tex.x = 0;
			v[i].tex.y = 0;
			v[i].col0   = colour;
			++i;
		}
	}

	inline void PrimitiveBatch::draw_line( const Line3& line, const ColourRGBA8& colour/*=ColourARGB8::White()*/ )
	{
		pr_start_new_batch(BatchType::Line3D);

		BatchVertex* v = m_vertices.alloc_unsafe(2);

		if( v && m_vertices.data() )
		{
			v[0].pos.x = line.start.x;
			v[0].pos.y = line.start.y;
			v[0].pos.z = line.start.z;
			v[0].tex.x = 0;
			v[0].tex.y = 0;
			v[0].col0   = colour;

			v[1].pos.x = line.end.x;
			v[1].pos.y = line.end.y;
			v[1].pos.z = line.end.z;
			v[1].tex.x = 0;
			v[1].tex.y = 0;
			v[1].col0   = colour;
		}
	}

	inline void PrimitiveBatch::draw_line( const Line3& line, const ColourRGBA8& colour_start, const ColourRGBA8& colour_end )
	{
		pr_start_new_batch(BatchType::Line3D);

		BatchVertex* v = m_vertices.alloc_unsafe(2);

		if( v && m_vertices.data() )
		{
			v[0].pos.x = line.start.x;
			v[0].pos.y = line.start.y;
			v[0].pos.z = line.start.z;
			v[0].tex.x = 0;
			v[0].tex.y = 0;
			v[0].col0   = colour_start;

			v[1].pos.x = line.end.x;
			v[1].pos.y = line.end.y;
			v[1].pos.z = line.end.z;
			v[1].tex.x = 0;
			v[1].tex.y = 0;
			v[1].col0   = colour_end;
		}
	}

	inline void PrimitiveBatch::draw_line( const Line2& line, const ColourRGBA8& colour_start, const ColourRGBA8& colour_end )
	{
		pr_start_new_batch(BatchType::Line);

		BatchVertex* v = m_vertices.alloc_unsafe(2);

		if( v && m_vertices.data() )
		{
			v[0].pos.x = line.start.x;
			v[0].pos.y = line.start.y;
			v[0].pos.z = 0;
			v[0].tex.x = 0;
			v[0].tex.y = 0;
			v[0].col0   = colour_start;

			v[1].pos.x = line.end.x;
			v[1].pos.y = line.end.y;
			v[1].pos.z = 0;
			v[1].tex.x = 0;
			v[1].tex.y = 0;
			v[1].col0   = colour_end;
		}
	}

	inline void PrimitiveBatch::draw_line( const Line2& line, const ColourRGBA8& colour)
	{
		pr_start_new_batch(BatchType::Line);

		BatchVertex* v = m_vertices.alloc_unsafe(2);

		if( v && m_vertices.data() )
		{
			v[0].pos.x = line.start.x;
			v[0].pos.y = line.start.y;
			v[0].pos.z = 0;
			v[0].tex.x = 0;
			v[0].tex.y = 0;
			v[0].col0   = colour;

			v[1].pos.x = line.end.x;
			v[1].pos.y = line.end.y;
			v[1].pos.z = 0;
			v[1].tex.x = 0;
			v[1].tex.y = 0;
			v[1].col0   = colour;
		}
	}

	inline void PrimitiveBatch::draw_cross(const Vector3& pos, float size, const ColourRGBA8& colour)
	{
		draw_line(Line3(pos+Vector3(-0.5f,0,0)*size, pos+Vector3(+0.5f,0,0)*size), colour);
		draw_line(Line3(pos+Vector3(0,-0.5f,0)*size, pos+Vector3(0,+0.5f,0)*size), colour);
		draw_line(Line3(pos+Vector3(0,0,-0.5f)*size, pos+Vector3(0,0,+0.5f)*size), colour);
	}

}
