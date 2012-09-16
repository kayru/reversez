#include "GfxVertexFormat.h"

namespace Rush
{
	static uint16 data_type_to_size( VertexFormatDescr::DataType::type type )
	{
		switch(type)
		{
		case VertexFormatDescr::DataType::Float1:		return 4;
		case VertexFormatDescr::DataType::Float2:		return 8;
		case VertexFormatDescr::DataType::Float3:		return 12;
		case VertexFormatDescr::DataType::Float4:		return 16;
		case VertexFormatDescr::DataType::Half2:		return 4;
		case VertexFormatDescr::DataType::Half4:		return 8;
		case VertexFormatDescr::DataType::Short2:		return 4;
		case VertexFormatDescr::DataType::Short2N:		return 4;
		case VertexFormatDescr::DataType::UByte4:		return 4;
		case VertexFormatDescr::DataType::Dec3N:		return 4;
		case VertexFormatDescr::DataType::Colour:		return 4;
		default:
			return 0;
		}
	}

	VertexFormatDescr::VertexFormatDescr() 
		: m_has_position(false)
		, m_has_normal(false)
		, m_has_colour(false)
	{
		for( uint32 i=0; i<MaxStreams; ++i )
		{
			m_stream_offset[i] = 0;
		}
		for( uint32 i=0; i<MaxElements; ++i )
		{
			m_elements[i] = Element(0,DataType::Unused, UsageType::Unused,0);
		}
	}

	void VertexFormatDescr::add( Element element )
	{
		if( element.usage == UsageType::Position ) m_has_position = true;
		if( element.usage == UsageType::Normal )   m_has_normal = true;
		if( element.usage == UsageType::Colour )   m_has_colour = true;
				
		element.offset = m_stream_offset[element.stream];

		m_stream_offset[element.stream] = m_stream_offset[element.stream] + element.size;

		m_elements.push_back(element);
	}

	VertexFormatDescr::Element::Element( uint16 _stream, DataType::type _type, UsageType::type _usage, uint8 _index ) 
		: stream(_stream)
		, size(data_type_to_size(_type))
		, offset(0)
		, index(_index)
		, type(_type)
		, usage(_usage)
	{

	}
}
