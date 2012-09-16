#pragma once

#include "Rush.h"

#include "UtilDynamicArray.h"
#include "UtilString.h"
#include "UtilDataStream.h"
#include "UtilReferenceCount.h"

#include <stdio.h>


// TODO: Move FileIn and FileOut to RLib. 
// This file should contain only the virtual filesystem functionality.

namespace Rush
{	
	class FileBase : public DataStream
	{

	public: 

		FileBase(): m_file(NULL) {};
		virtual ~FileBase(){};

		virtual uint32	tell();
		virtual void	seek(uint32 pos);
		virtual void	skip(int32 distance);
		virtual void	rewind();

		virtual bool	valid();

	protected:

		FILE* m_file;

	private:

	};

	//////////////////////////////////////////////////////////////////////////

	class FileIn : public FileBase
	{

	public: 

		FileIn(const char* filename);
		virtual ~FileIn();

		virtual uint32	read (void* buf, uint32 size);
		virtual uint32	write(const void* /*buf*/, uint32 /*size*/) {return 0;}

		virtual uint32  length();

	protected:

	private:

	};

	//////////////////////////////////////////////////////////////////////////

	class FileOut : public FileBase
	{

	public: 

		FileOut(const char* filename, uint32 buffer_size=(1<<20));
		virtual ~FileOut();		

		virtual uint32	read (void* /*buf*/, uint32 /*size*/) { return 0; }
		virtual uint32	write(const void* buf, uint32 size);

		virtual uint32  length() { return 0; }

		void			close();

	protected:		

		void flush();

	private:

		FileOut& operator=(const FileOut&);

		char*			m_buffer;
		const uint32	m_buffer_size;
		uint32			m_buffer_pos;

	};

	//////////////////////////////////////////////////////////////////////////

	class FileSystem : public ReferenceCount
	{	
	public:

		FileSystem();
		~FileSystem();

		DataStream* open(const char* filename);
		void string_from_file(const char* filename, String& result);

		void add_path(const char* path);

	private:

		DynamicArray<String> m_path_list;

	};

}
