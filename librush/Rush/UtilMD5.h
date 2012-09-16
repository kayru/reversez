#pragma once

/* MD5
 converted to C++ class by Frank Thilo (thilo@unix-ag.org)
 for bzflag (http://www.bzflag.org)

   based on:

   md5.h and md5.c
   reference implementation of RFC 1321

   Copyright (C) 1991-2, RSA Data Security, Inc. Created 1991. All
rights reserved.

License to copy and use this software is granted provided that it
is identified as the "RSA Data Security, Inc. MD5 Message-Digest
Algorithm" in all material mentioning or referencing this software
or this function.

License is also granted to make and use derivative works provided
that such works are identified as "derived from the RSA Data
Security, Inc. MD5 Message-Digest Algorithm" in all material
mentioning or referencing the derived work.

RSA Data Security, Inc. makes no representations concerning either
the merchantability of this software or the suitability of this
software for any particular purpose. It is provided "as is"
without express or implied warranty of any kind.

These notices must be retained in any copies of any part of this
documentation and/or software.

*/

#include "Rush.h"
#include "UtilHash.h"

namespace Rush
{

	/// @addtogroup util
	/// @{
	
	class Hasher_MD5
	{
	public:
		Hasher_MD5();
		Hasher_MD5(const char* data, uint32 length);
		void update(const unsigned char *buf, uint32 length);
		void update(const char *buf, uint32 length);
		Hasher_MD5& finalize();

		MD5Hash get();

	private:

		void init();

		enum {blocksize = 64}; // VC6 won't eat a const static int here

		void transform(const uint8 block[blocksize]);
		static void decode(uint32 output[], const uint8 input[], uint32 len);
		static void encode(uint8 output[], const uint32 input[], uint32 len);

		bool finalized;
		uint8 buffer[blocksize]; // bytes that didn't fit in last 64 byte chunk
		uint32 count[2];   // 64bit counter for number of bits (lo, hi)
		uint32 state[4];   // digest so far
		uint8 digest[16]; // the result

		// low level logic operations
		static inline uint32 F(uint32 x, uint32 y, uint32 z);
		static inline uint32 G(uint32 x, uint32 y, uint32 z);
		static inline uint32 H(uint32 x, uint32 y, uint32 z);
		static inline uint32 I(uint32 x, uint32 y, uint32 z);
		static inline uint32 rotate_left(uint32 x, int n);
		static inline void FF(uint32 &a, uint32 b, uint32 c, uint32 d, uint32 x, uint32 s, uint32 ac);
		static inline void GG(uint32 &a, uint32 b, uint32 c, uint32 d, uint32 x, uint32 s, uint32 ac);
		static inline void HH(uint32 &a, uint32 b, uint32 c, uint32 d, uint32 x, uint32 s, uint32 ac);
		static inline void II(uint32 &a, uint32 b, uint32 c, uint32 d, uint32 x, uint32 s, uint32 ac);
	};

	/// @}

}


