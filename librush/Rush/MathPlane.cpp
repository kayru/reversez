
#include "MathPlane.h"
#include <math.h> // for sqrt

namespace Rush
{
	bool Plane::intersect3( const Plane& a, const Plane& b, const Plane& c, Vector3& out_point )
	{
		Vector3 num = (b.cross(c)) * a.w + (c.cross(a)) * b.w  + (a.cross(b)) * c.w;
		float den = a.dot(b.cross(c));

		if( den != 0.0f )
		{
			out_point = -num/den;
			return true;
		}
		else
		{
			return false;
		}
	}	

	Vector3 Plane::intersect_infinite_line( const Vector3& a, const Vector3& b )
	{
		float da = this->distance(a);
		float db = this->distance(b);

		if( (da-db) == 0.0f ) 
		{
			return a;
		}
		else
		{
			return a + ((b-a) * (-da/(db-da)));
		}

	}

	void Plane::normalize_fast()
	{
		float len_sq = x*x + y*y + z*z;
		float rlen = rsqrt_fast(len_sq);

		x*=rlen;
		y*=rlen;
		z*=rlen;
		w*=rlen;
	}

	void Plane::normalize()
	{
		float len_sq = x*x + y*y + z*z;
		float rlen = 1.0f / sqrt(len_sq);

		x*=rlen;
		y*=rlen;
		z*=rlen;
		w*=rlen;
	}
}
