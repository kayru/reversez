#include "UtilImage.h"
#include "UtilLog.h"
#include "MathPixel.h"
#include "Filesystem.h"

namespace Rush
{
#pragma pack(push)
#pragma pack(1)
	struct DDSHeader // based on ddraw.h
	{		
		static const uint32 DDPF_ALPHAPIXELS	= 0x00000001;
		static const uint32 DDPF_FOURCC			= 0x00000004;
		static const uint32 DDPF_RGB			= 0x00000040;
		static const uint32 DDPF_LUMINANCE		= 0x00020000;

		static const uint32 DDPF_RGBA			= DDPF_RGB|DDPF_ALPHAPIXELS;

		uint32 dwMagic;
		uint32 dwSize;
		uint32 dwFlags;
		uint32 dwHeight;
		uint32 dwWidth;
		uint32 dwPitchOrLinearSize;
		uint32 dwDepth; 
		uint32 dwMipMapCount;
		uint32 dwReserved[11];

		struct 
		{
			uint32 dwSize;
			uint32 dwFlags;
			uint32 dwFourCC;
			uint32 dwRGBBitCount;
			uint32 dwRBitMask;
			uint32 dwGBitMask;
			uint32 dwBBitMask;
			uint32 dwRGBAlphaBitMask; 
		} ddpfPixelFormat;

		struct 
		{
			uint32 dwCaps1;
			uint32 dwCaps2;
			uint32 Reserved[2];
		} ddsCaps;

		uint32 dwReserved2;
	};

    struct TGAHeader
    {
        uint8  identsize;          // size of ID field that follows 18 byte header (0 usually)
        uint8  colourmaptype;      // type of colour map 0=none, 1=has palette
        uint8  imagetype;          // type of image 0=none,1=indexed,2=rgb,3=grey,+8=rle packed
        
        short colourmapstart;     // first colour map entry in palette
        short colourmaplength;    // number of colours in palette
        uint8  colourmapbits;      // number of bits per palette entry 15,16,24,32
        
        short xstart;             // image x origin
        short ystart;             // image y origin
        short width;              // image width in pixels
        short height;             // image height in pixels
        uint8  bits;               // image bits per pixel 8,16,24,32
        uint8  descriptor;         // image descriptor bits (vh flip bits)
        
        // pixel data follows header
        
    };
    
    
#pragma pack(pop)

	inline uint32 make_fourcc(char a, char b, char c, char d)
	{
		return (a | (b << 8) | (c << 16) | (d << 24));
	}

	uint32 PixelFormat::bits_per_pixel( PixelFormat::type fmt )
	{
		switch(fmt)
		{
		case A8R8G8B8:			return 32;
		case B8G8R8A8:			return 32;                
		case A16B16G16R16:		return 64;
		case A16B16G16R16F:		return 64;
		case A32B32G32R32F:		return 128;
		case G16R16F:			return 32;
		case G32R32F:			return 64;
		case R16F:				return 16;
		case R32F:				return 32;
		case D24S8:				return 32;

		case L8:				return 8;
		case L16:				return 8;

		case DXT1:				return 4;
		case DXT5:				return 8;

		default: return 0;
		}	
	}

	const char* PixelFormat::to_string( PixelFormat::type fmt )
	{
		switch(fmt)
		{
		case A8R8G8B8:			return "A8R8G8B8";
		case B8G8R8A8:			return "B8G8R8A8";
		case A16B16G16R16:		return "A16B16G16R16";
		case A16B16G16R16F:		return "A16B16G16R16F";
		case A32B32G32R32F:		return "A32B32G32R32F";
		case G16R16F:			return "G16R16F";
		case G32R32F:			return "G32R32F";
		case R16F:				return "R16F";
		case R32F:				return "R32F";
		case D24S8:				return "D24S8";

		case L8:				return "L8";
		case L16:				return "L16";

		case DXT1:				return "DXT1";
		case DXT5:				return "DXT5";

		default:				return "Unknown";
		}	
	}

	//////////////////////////////////////////////////////////////////////////

	Image::Image()
		: m_format(PixelFormat::Unknown)
		, m_width(0)
		, m_height(0)
		, m_data(NULL)
        , m_data_size_in_bytes(0)
	{
	}

	Image::~Image()
	{
		delete m_data;
	}

	bool Image::read_dds( DataStream& stream )
	{	
		// reset image
		
		m_format = PixelFormat::Unknown;
		m_width = 0;
		m_height = 0;
		delete m_data;
		m_data = NULL;
				
		// load header

		DDSHeader header = {0};		
		stream.read(&header, sizeof(header));
				
		if (header.dwMagic != make_fourcc('D','D','S',' '))
		{			
			Log::error("Image::read_dds() DDS file identifier not found.");
			return false;
		}
		
		// detect format

		if( header.ddpfPixelFormat.dwFlags == DDSHeader::DDPF_RGBA && 
            header.ddpfPixelFormat.dwRGBBitCount == 32 ) 
		{
			m_format = PixelFormat::A8R8G8B8;
		}

		// read pixels
        
		m_width  = header.dwWidth;
		m_height = header.dwHeight;

		uint32 size_in_bits = m_width * m_height * header.ddpfPixelFormat.dwRGBBitCount;
		uint32 size_in_bytes = size_in_bits/8;

		m_data = new uint8[size_in_bytes];
        m_data_size_in_bytes = size_in_bytes;
		
		stream.read(m_data, size_in_bytes);
		
		return true;
	}

	bool Image::load_dds( const char* filename )
	{
		FileIn fi(filename);
		if( fi.valid() )
		{			
			return read_dds(fi);
		}
		else
		{	
            Log::error("File not found: '%s'", filename);
			return false;
		}
	}
    
    bool Image::create(uint32 w, uint32 h, PixelFormat::type format)
    {
        delete m_data;
        
        m_format = format;
        
        m_width  = w;
        m_height = h;
        
        uint32 size_in_bytes = m_width * m_height * PixelFormat::bits_per_pixel(m_format) / 8;
        
        m_data = new uint8[size_in_bytes];
        m_data_size_in_bytes = size_in_bytes;
        
        return true;
    }
    
    bool Image::save_tga(const char* filename)
    {
        if( m_format!=PixelFormat::B8G8R8A8 )
        {
            Log::error("Saving to TGA only supported for B8G8R8A8 format.");
            return false;
        }
        
        TGAHeader hdr;
        
        hdr.identsize = 0;              // size of ID field that follows 18 byte header (0 usually)
        hdr.colourmaptype = 0;          // type of colour map 0=none, 1=has palette
        hdr.imagetype = 2;              // type of image 0=none,1=indexed,2=rgb,3=grey,+8=rle packed
        
        hdr.colourmapstart = 0;         // first colour map entry in palette
        hdr.colourmaplength = 0;        // number of colours in palette
        hdr.colourmapbits = 0;          // number of bits per palette entry 15,16,24,32
        
        hdr.xstart = 0;                 // image x origin
        hdr.ystart = 0;                 // image y origin
        hdr.width = (short)m_width;     // image width in pixels
        hdr.height = (short)m_height;   // image height in pixels
        hdr.bits = 32;                  // image bits per pixel 8,16,24,32
        hdr.descriptor = 1<<5;          // image descriptor bits (vh flip bits)        

        FileOut fo(filename);
        if( fo.valid() == false ) 
        {
            Log::error("Failed to write file '%s'", filename);
            return false;    
        }
        
        uint32 bw = 0;
        
        bw += fo.write_t(hdr);        
        bw += fo.write(m_data, m_data_size_in_bytes);
                
        R_ASSERT(bw == sizeof(hdr) + m_data_size_in_bytes);
        
        return true;
    }    
    
}

