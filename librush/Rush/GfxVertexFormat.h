#pragma once

#include "Rush.h"
#include "UtilStaticArray.h"

namespace Rush
{
	class VertexFormatDescr
	{

		static const uint32 MaxStreams  = 8;
		static const uint32 MaxElements = 16;

	public:

		struct DataType
		{
			typedef uint8 type;
			static const uint8 Unused = 0;
			static const uint8 Float1 = 1;
			static const uint8 Float2 = 2;
			static const uint8 Float3 = 3;
			static const uint8 Float4 = 4;
			static const uint8 Half2 = 5;
			static const uint8 Half4 = 6;
			static const uint8 Short2 = 7;
			static const uint8 Short2N = 8;
			static const uint8 UByte4 = 9;
			static const uint8 Dec3N = 10;
			static const uint8 Colour = 11;
		};

		struct UsageType
		{
			typedef uint8 type;
			static const uint8 Unused = 0;
			static const uint8 Position = 1;
			static const uint8 Texcoord = 2;
			static const uint8 Colour = 3;
			static const uint8 Normal = 4;
			static const uint8 Binormal = 5;
			static const uint8 Tangent = 6;
		};

		struct Element
		{
			Element(){}
			Element(uint16 _stream, DataType::type _type, UsageType::type _usage, uint8 _index);

			uint16				stream;
			uint16				size;
			uint16				offset;
			uint8				index;
			DataType::type		type;
			UsageType::type		usage;

			bool operator == (const Element& rhs) const 
			{
				return stream == rhs.stream 
					&& type == rhs.type
					&& usage == rhs.usage
					&& index == rhs.index;
			}
		};

	public:

		VertexFormatDescr();

		inline const Element&		element(uint32 n)		const	{ return m_elements[n]; }
		inline size_t				num_elements()			const	{ return m_elements.size(); }
		inline uint16				stream_stride(uint32 n)	const	{ return m_stream_offset[n]; }

		inline bool					has_position() const { return m_has_position; }
		inline bool					has_normal()   const { return m_has_normal; }
		inline bool					has_colour()   const { return m_has_colour; }

		bool operator == (const VertexFormatDescr& rhs) const 
		{			
			if( num_elements() == rhs.num_elements() )
			{
				for( uint32 i=0; i<num_elements(); ++i )
				{
					if( !(m_elements[i]==rhs.m_elements[i]) ) return false;
				}
				return true;
			}
			else
			{
				return false;
			}
		}

		inline void add(uint16 _stream, DataType::type _type, UsageType::type _usage, uint8 _index)
		{
			Element elem(_stream, _type, _usage, _index);
			add(elem);
		}

		void add(Element element);
		
	private:

		StaticArray<Element, MaxElements> m_elements;

		uint16 m_stream_offset[MaxStreams];

		bool m_has_position;
		bool m_has_normal;
		bool m_has_colour;
	};

}
