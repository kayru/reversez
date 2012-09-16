#pragma once

#include "Rush.h"

namespace Rush
{

	/// @addtogroup util
	/// @{
	
	class DataStream
	{

	public: 

		virtual ~DataStream() {}

		virtual uint32	write(const void* /*buf*/, uint32 /*size*/) = 0;
		template <typename T>
		INLINE uint32	write_t(const T& val) { return write(&val, sizeof(val)); }

		virtual uint32	read (void* /*buf*/, uint32 /*size*/) = 0;
		template <typename T>
		INLINE uint32	read_t(T& val) { return read(&val, sizeof(val)); }

		virtual uint32	tell()						= 0;
		virtual void	seek(uint32 /*pos*/)		= 0;
		virtual void	skip(int32 /*distance*/)	= 0;
		virtual void	rewind()					= 0;

		virtual bool	valid()						= 0;

		virtual uint32  length()					= 0;

	};

	class MemDataStream : public DataStream
	{
	public: 

		MemDataStream(const void* data, uint32 size);
		MemDataStream(void* data, uint32 size);

		virtual uint32	write(const void* buf, uint32 size);
		virtual uint32	read(void* buf, uint32 size);

		virtual uint32	tell();
		virtual void	seek(uint32 pos);
		virtual void	skip(int32 distance);
		virtual void	rewind();

		virtual bool	valid();

		virtual uint32  length() { return m_size; }

	private:

		uint8*			m_data_rw;
		const uint8*	m_data_ro;

		uint32  m_pos;
		uint32  m_size;

	};

	class DummyDataStream : public DataStream
	{
	public: 

		DummyDataStream()
			: m_pos(0)
			, m_size(0)
		{}

		virtual uint32	write(const void* /*buf*/, uint32 size) { m_pos += size; return m_pos; }
		virtual uint32	read(void* /*buf*/, uint32 size) { m_pos += size; return m_pos; }

		virtual uint32	tell() { return m_pos; }
		virtual void	seek(uint32 pos) { m_pos = pos; }
		virtual void	skip(int32 distance) { m_pos += distance; }
		virtual void	rewind() { m_pos = 0; }

		virtual bool	valid() { return true; }

		virtual uint32  length() { return m_size; }

	private:

		uint32  m_pos;
		uint32  m_size;

	};

	/// @}

}
