#include "MathSphericalHarmonics.h"
#include "MathConstants.h"

#include <math.h>

namespace Rush
{
	static const float g_sh1 = 0.282095f; // 0.5*sqrt(1/pi)
	static const float g_sh2 = 0.488603f; // 0.5*sqrt(3/pi)
	static const float g_sh3 = 1.092548f; // 0.5*sqrt(15/pi)
	static const float g_sh4 = 0.315392f; // 0.25*sqrt(5/pi)
	static const float g_sh5 = 0.546274f; // 

	SphericalHarmonics3::SphericalHarmonics3()
	{
		m_coefficients[0] = 0;
		m_coefficients[1] = 0;
		m_coefficients[2] = 0;
		m_coefficients[3] = 0;
		m_coefficients[4] = 0;
		m_coefficients[5] = 0;
		m_coefficients[6] = 0;
		m_coefficients[7] = 0;
		m_coefficients[8] = 0;
	}

	SphericalHarmonics3::SphericalHarmonics3( float* r, float* g, float* b )
	{
		for( uint32 i=0; i<9; ++i )
		{
			m_coefficients[i] = Vector4(r[i], g[i], b[i], 0.0f);
		}
	}

	void SphericalHarmonics3::add( const Vector3& dir, const Vector3& colour_ )
	{		
		float w[9];
		weights(w, dir*Vector3(-1,-1,1));

		Vector4 colour(colour_,0);
		
		m_coefficients[0]+=colour*w[0];
		m_coefficients[1]+=colour*w[1];
		m_coefficients[2]+=colour*w[2];
		m_coefficients[3]+=colour*w[3];
		m_coefficients[4]+=colour*w[4];
		m_coefficients[5]+=colour*w[5];
		m_coefficients[6]+=colour*w[6];
		m_coefficients[7]+=colour*w[7];
		m_coefficients[8]+=colour*w[8];
	}

	void SphericalHarmonics3::add( const SphericalHarmonics3& rhs )
	{
		for( uint32 i=0; i<9; ++i )
		{
			m_coefficients[i] += rhs.m_coefficients[i];
		}
	}

	void SphericalHarmonics3::add( const Vector3& colour )
	{
		m_coefficients[0].xyz() += colour;
	}
	Vector4 SphericalHarmonics3::evaluate( const Vector3& dir )
	{
		float w[9];
		weights(w, dir);
		
		Vector4 res;
		res  = m_coefficients[0] * w[0];
		res += m_coefficients[1] * w[1];
		res += m_coefficients[2] * w[2];
		res += m_coefficients[3] * w[3];
		res += m_coefficients[4] * w[4];
		res += m_coefficients[5] * w[5];
		res += m_coefficients[6] * w[6];
		res += m_coefficients[7] * w[7];
		res += m_coefficients[8] * w[8];

		return res;
	}

	void SphericalHarmonics3::mul( float s )
	{
		m_coefficients[0] *= s;
		m_coefficients[1] *= s;
		m_coefficients[2] *= s;
		m_coefficients[3] *= s;
		m_coefficients[4] *= s;
		m_coefficients[5] *= s;
		m_coefficients[6] *= s;
		m_coefficients[7] *= s;
		m_coefficients[8] *= s;
	}

	void SphericalHarmonics3::weights( float* out, const Vector3& indir)
	{
		// order as per http://www.cs.columbia.edu/~cs4162/slides/spherical-harmonic-lighting.pdf
		Vector3 dir = -indir;
		out[0] = g_sh1;
		out[1] = g_sh2*(dir.y);
		out[2] = g_sh2*(dir.z);
		out[3] = g_sh2*(dir.x);				
		out[4] = g_sh3*(dir.y*dir.x);
		out[5] = g_sh3*(dir.y*dir.z);
		out[6] = g_sh4*(3.0f * dir.z * dir.z - 1.0f);
		out[7] = g_sh3*(dir.z*dir.x);
		out[8] = g_sh5*(dir.x * dir.x - dir.y * dir.y);
	}

	void SphericalHarmonics3::generate_constants_opt( Vector4* out ) const
	{
		const Vector4* c = m_coefficients;

		const float r[] = { c[0].x, c[1].x, c[2].x, c[3].x, c[4].x, c[5].x, c[6].x, c[7].x, c[8].x };
		const float g[] = { c[0].y, c[1].y, c[2].y, c[3].y, c[4].y, c[5].y, c[6].y, c[7].y, c[8].y };
		const float b[] = { c[0].z, c[1].z, c[2].z, c[3].z, c[4].z, c[5].z, c[6].z, c[7].z, c[8].z };

		const float* fLight[3] = { r, g, b };

		static const float s_fSqrtPI = ( ( float )sqrt( PI ) );
		const float fC0 = 1.0f / ( 2.0f * s_fSqrtPI );
		const float fC1 = ( float )sqrt( 3.0f ) / ( 3.0f * s_fSqrtPI );
		const float fC2 = ( float )sqrt( 15.0f ) / ( 8.0f * s_fSqrtPI );
		const float fC3 = ( float )sqrt( 5.0f ) / ( 16.0f * s_fSqrtPI );
		const float fC4 = 0.5f * fC2;

		for( int i = 0; i < 3; i++ )
		{
			out[0+i].x = -fC1 * fLight[i][3];
			out[0+i].y = -fC1 * fLight[i][1];
			out[0+i].z = fC1 * fLight[i][2];
			out[0+i].w = fC0 * fLight[i][0] - fC3 * fLight[i][6];

			out[3+i].x = fC2 * fLight[i][4];
			out[3+i].y = -fC2 * fLight[i][5];
			out[3+i].z = 3.0f * fC3 * fLight[i][6];
			out[3+i].w = -fC2 * fLight[i][7];
		}
		out[6].x = fC4 * fLight[0][8];
		out[6].y = fC4 * fLight[1][8];
		out[6].z = fC4 * fLight[2][8];
		out[6].w = 1.0f;
	}

	void SphericalHarmonics3::generate_constants( Vector4* out ) const
	{	
		const Vector4* c = m_coefficients;
		Vector4 s[9];

		float w1 = 1.0f;
		float w2 = 2.0f/3.0f;
		float w3 = 1.0f/4.0f;

		s[0] = c[0] * g_sh1 * w1;

		s[1] = c[1] * g_sh2 * w2;
		s[2] = c[2] * g_sh2 * w2 * -1.0f;
		s[3] = c[3] * g_sh2 * w2;

		s[4] = c[4] * g_sh3 * w3;
		s[5] = c[5] * g_sh3 * w3 * -1.0f;
		s[6] = c[6] * g_sh4 * w3;
		s[7] = c[7] * g_sh3 * w3 * -1.0f;
		s[8] = c[8] * g_sh5 * w3;
		
		const float r[] = { s[0].x, s[1].x, s[2].x, s[3].x, s[4].x, s[5].x, s[6].x, s[7].x, s[8].x };
		const float g[] = { s[0].y, s[1].y, s[2].y, s[3].y, s[4].y, s[5].y, s[6].y, s[7].y, s[8].y };
		const float b[] = { s[0].z, s[1].z, s[2].z, s[3].z, s[4].z, s[5].z, s[6].z, s[7].z, s[8].z };

		// Red
		out[0] = Vector4(r[0], g[0], b[0], r[7]);
		out[1] = Vector4(r[1], g[1], b[1], g[7]);
		out[2] = Vector4(r[2], g[2], b[2], b[7]);
		out[3] = Vector4(r[3], g[3], b[3], r[8]);
		out[4] = Vector4(r[4], g[4], b[4], g[8]);
		out[5] = Vector4(r[5], g[5], b[5], b[8]);
		out[6] = Vector4(r[6], g[6], b[6], 0);				
	}

	void SphericalHarmonics3::lerp( const SphericalHarmonics3& target, float a )
	{
		float b = 1.0f-a;
		for(uint32 i=0; i<9; ++i)
		{
			m_coefficients[i] = m_coefficients[i]*b + target.m_coefficients[i]*a;
		}
	}


}
// EOF
