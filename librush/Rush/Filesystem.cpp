#include "Filesystem.h"
#include "MathCommon.h"

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4996) // 'fopen': This function or variable may be unsafe. Consider using fopen_s instead.
#endif //_MSC_VER

namespace Rush
{
	uint32 FileIn::read( void* buf, uint32 size )
	{
		uint32 br = (uint32)fread(buf, 1, size, m_file);
		return br;
	}

	FileIn::FileIn( const char* filename )
	{
		m_file = fopen(filename, "rb");
	}

	FileIn::~FileIn()
	{
		if( m_file )
		{
			fclose(m_file);
			m_file = NULL;
		}
	}

	uint32 FileIn::length()
	{
		uint32 curr_pos = tell();

		fseek(m_file, 0, SEEK_END);

		uint32 end_pos = tell();

		fseek(m_file, curr_pos, SEEK_SET);

		return end_pos;
	}

	//////////////////////////////////////////////////////////////////////////

	uint32 FileOut::write(const void* buf, uint32 size)
	{
		uint32 in_buf_pos	 = 0;
		uint32 in_bytes_left = size;

		while(in_bytes_left!=0)
		{
			uint32 out_bytes_left = m_buffer_size - m_buffer_pos;

			uint32 bytes_to_copy = rush_min(in_bytes_left, out_bytes_left);

			rush_memcpy(&m_buffer[m_buffer_pos], &((char*)buf)[in_buf_pos], bytes_to_copy);

			m_buffer_pos	+= bytes_to_copy;
			in_buf_pos		+= bytes_to_copy;

			in_bytes_left	-= bytes_to_copy;

			if( in_bytes_left!=0 || out_bytes_left==0)
			{
				flush();
			}
		}
		return size;
	}

	FileOut::FileOut( const char* filename, uint32 buffer_size ):
		m_buffer(NULL),
		m_buffer_size(buffer_size),
		m_buffer_pos(0)
	{
		m_file = fopen(filename, "wb");
		m_buffer = new char[m_buffer_size];
	}

	FileOut::~FileOut()
	{
		if( m_file ) 
		{
			close();
		}
	}

	void FileOut::flush()
	{
		fwrite(m_buffer, 1, m_buffer_pos, m_file);
		m_buffer_pos = 0;		
	}

	void FileOut::close()
	{
		flush();

		fclose(m_file);
		m_file = NULL;
		
		delete m_buffer;
		m_buffer = NULL;
	}

	//////////////////////////////////////////////////////////////////////////

	uint32 FileBase::tell()
	{
		return (uint32)ftell(m_file); // TODO: fix casting
	}

	void FileBase::seek( uint32 pos )
	{
		fseek(m_file, pos, SEEK_SET);
	}

	void FileBase::skip( int32 distance )
	{
		fseek(m_file, distance, SEEK_CUR);
	}

	void FileBase::rewind()
	{
		fseek(m_file, 0, SEEK_SET);
	}

	bool FileBase::valid()
	{
		return m_file != NULL;
	}
	
	//////////////////////////////////////////////////////////////////////////

	FileSystem::FileSystem()
	{

	}

	FileSystem::~FileSystem()
	{

	}

	DataStream* FileSystem::open( const char* filename )
	{		
		FileIn* fi = new FileIn(filename);
		if( fi->valid() )
		{
			return fi;
		}
		else
		{
			delete fi;
			return NULL;
		}
	}

	void FileSystem::string_from_file( const char* filename, String& result )
	{
		DataStream* ds = open(filename);

		if( ds == NULL ) return;
		if( ds->valid() == false ) return;

		uint32 len = ds->length();

		result.resize(len+1);

		uint32 br = ds->read(&result[0], len); 
		result[br] = 0x00;
		delete ds;
	}
}

#ifdef _MSC_VER
#pragma warning(pop)
#endif //_MSC_VER
