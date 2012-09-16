#pragma once
namespace Rush
{
	struct EmbeddedFont
	{
		static unsigned char		header_bytes[];
		static unsigned long		header_size;
		static unsigned char		pixels_bytes[];
		static unsigned long		pixels_size;
		static const unsigned long	width  = 256;
		static const unsigned long	height = 64;
	};
}
