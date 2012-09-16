#include "UtilSHA1.h"
#include "UtilDynamicArray.h"
#include <string.h>

namespace Rush
{
	Hasher_SHA1::Hasher_SHA1()	
	{
		m_hash[0] = 0x67452301;
		m_hash[1] = 0xEFCDAB89;
		m_hash[2] = 0x98BADCFE;
		m_hash[3] = 0x10325476;
		m_hash[4] = 0xC3D2E1F0;
	}

	static void set_length_bits(uint8* message, uint64 length_bits)
	{
		union
		{
			struct
			{
				uint32 low;
				uint32 high;
			} separate;

			uint64 combined;
		} bits;

		
		bits.combined = length_bits;
		
		message[0] = (bits.separate.high >> 24) & 0xFF;
		message[1] = (bits.separate.high >> 16) & 0xFF;
		message[2] = (bits.separate.high >> 8) & 0xFF;
		message[3] = (bits.separate.high) & 0xFF;
		message[4] = (bits.separate.low >> 24) & 0xFF;
		message[5] = (bits.separate.low >> 16) & 0xFF;
		message[6] = (bits.separate.low >> 8) & 0xFF;
		message[7] = (bits.separate.low) & 0xFF;
	}
	
	void Hasher_SHA1::hash( const uint8* message, uint32 length )
	{
		uint64 orig_length_bits = ((uint64)length)*8;

		while(length>64)
		{
			process_block(message);
			message	= &message[64];
			length -= 64;			
		}

		uint8 padded_block[65];
		rush_memcpy(padded_block, message, length);

		padded_block[length] = 0x80;			
		for(uint32 i=length+1; i<64; ++i )
		{
			padded_block[i] = 0;
		}

		if( length >= 56 )
		{											
			process_block(padded_block);
			rush_memset(padded_block, 0, 64);				
		}

		set_length_bits(&padded_block[56], orig_length_bits);
		process_block(padded_block);
		
	}

	void Hasher_SHA1::process_block( const uint8* block )
	{
		uint32 W[80];
		rush_memset(W, 0, sizeof(W));

		for(uint32 i=0; i<16; ++i)
		{			
			W[i]  = block[i*4 + 0] << 24;
			W[i] |= block[i*4 + 1] << 16;
			W[i] |= block[i*4 + 2] << 8;
			W[i] |= block[i*4 + 3] << 0;
		}

		for( uint32 i=16; i<80; ++i )
		{
			W[i] = rotate_left(W[i-3] ^ W[i-8] ^ W[i-14] ^ W[i-16], 1);
		}

		uint32 A = m_hash[0];
		uint32 B = m_hash[1];
		uint32 C = m_hash[2];
		uint32 D = m_hash[3];
		uint32 E = m_hash[4];

		for(uint32 i=0; i<20; ++i)
		{
			uint32 F = (B & C) | ((~B) & D);
			uint32 K = 0x5A827999;

			uint32 temp = rotate_left(A,5) + F + E + W[i] + K;

			E = D;
			D = C;
			C = rotate_left(B,30);
			B = A;
			A = temp;
		}

		for(uint32 i = 20; i < 40; ++i)
		{
			uint32 F = B ^ C ^ D;
			uint32 K = 0x6ED9EBA1;

			uint32 temp = rotate_left(A,5) + F + E + W[i] + K;
			
			E = D;
			D = C;
			C = rotate_left(B,30);
			B = A;
			A = temp;
		}

		for(uint32 i = 40; i < 60; ++i)
		{
			uint32 F = (B & C) | (B & D) | (C & D);
			uint32 K = 0x8F1BBCDC;

			uint32 temp = rotate_left(A,5) + F + E + W[i] + K;
			
			E = D;
			D = C;
			C = rotate_left(B,30);
			B = A;
			A = temp;
		}

		for(uint32 i = 60; i < 80; ++i)
		{
			uint32 F = B ^ C ^ D;
			uint32 K = 0xCA62C1D6;
			
			uint32 temp = rotate_left(A,5) + F + E + W[i] + K;
			
			E = D;
			D = C;
			C = rotate_left(B,30);
			B = A;
			A = temp;
		}

		m_hash[0] += A;
		m_hash[1] += B;
		m_hash[2] += C;
		m_hash[3] += D;
		m_hash[4] += E;
	}

	const SHA1_Result Hasher_SHA1::result()
	{
		SHA1_Result res; rush_memcpy(res.data, m_hash, sizeof(m_hash)); return res;
	}
}
