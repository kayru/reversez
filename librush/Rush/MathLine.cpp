#include "MathLine.h"

#include <math.h>

namespace Rush
{
	float Line2::distance( const Vector2& pos ) const
	{
		Vector2 v = end - start;
		Vector2 w = pos - start;

		float c1 = w.dot(v);
		if( c1<=0.0f ) return (pos-start).length();

		float c2 = v.dot(v);
		if( c2<=c1 ) return (pos-end).length();

		float b = c1 / c2;

		Vector2 pb = start + v*b;

		return (pos-pb).length();
	}

	int32 Line2::intersect( const Line2& rhs, Vector2* pos ) const
	{
		float d=(end.x-start.x)*(rhs.end.y-rhs.start.y)-(end.y-start.y)*(rhs.end.x-rhs.start.x);
		if (fabs(d)<0.0001f)
		{
			return -1;
		}
		float ab=((start.y-rhs.start.y)*(rhs.end.x-rhs.start.x)-(start.x-rhs.start.x)*(rhs.end.y-rhs.start.y))/d;
		if (ab>0.0 && ab<1.0)
		{
			float cd=((start.y-rhs.start.y)*(end.x-start.x)-(start.x-rhs.start.x)*(end.y-start.y))/d;
			if (cd>0.0 && cd<1.0)
			{ 
				if( pos )
				{
					pos->x=start.x+ab*(end.x-start.x);
					pos->y=start.y+ab*(end.y-start.y);
				}
				return 1;
			}
		}
		return 0;
	}
}

