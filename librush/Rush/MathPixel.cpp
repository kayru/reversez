
#include "MathPixel.h"
#include "MathCommon.h"

#include <math.h>

namespace Rush
{
	float epsilon = 0.00001f;
		
	ColourRGBA::operator ColourRGBA8() const
	{

		uint8 r8 = uint8(rush_saturate(r) * 255.0f);
		uint8 g8 = uint8(rush_saturate(g) * 255.0f);
		uint8 b8 = uint8(rush_saturate(b) * 255.0f);
		uint8 a8 = uint8(rush_saturate(a) * 255.0f);

		return ColourRGBA8(r8, g8, b8, a8);

	}
		
	void ColourRGBA::rgb_to_hsv()
	{
		float h = 0;
		float s = 0;
		float v = 0;

		float min, max, delta;
		min = rush_min( r, rush_min(g, b) );
		max = rush_max( r, rush_max(g, b) );
		v = max;				// v


		delta = max - min;
		if( max > epsilon )
			s = delta / max;		// s
		else {
			// r = g = b = 0		// s = 0, v is undefined
			// s = 0;
			// h = -1;
			return;
		}
		if( fabs(r-max)<epsilon )
			h = ( g - b ) / delta;		// between yellow & magenta
		else if( fabs(g-max)<epsilon )
			h = 2 + ( b - r ) / delta;	// between cyan & yellow
		else
			h = 4 + ( r - g ) / delta;	// between magenta & cyan
		h *= 60;				// degrees
		if( h < 0 )
			h += 360;

		r = h;
		g = s;
		b = v;
	}

	void ColourRGBA::hsv_to_rgb()
	{
		float h = r;
		float s = g;
		float v = b;

		int i;
		float f, p, q, t;
		if( s < epsilon ) // achromatic (grey) 
		{
			r = g = b;
			return;
		}

		h /= 60;			// sector 0 to 5
		i = int(floor( h ));
		f = h - i;			// factorial part of h
		p = v * ( 1 - s );
		q = v * ( 1 - s * f );
		t = v * ( 1 - s * ( 1 - f ) );
		switch( i ) {
		case 0:
			r = v;
			g = t;
			b = p;
			break;
		case 1:
			r = q;
			g = v;
			b = p;
			break;
		case 2:
			r = p;
			g = v;
			b = t;
			break;
		case 3:
			r = p;
			g = q;
			b = v;
			break;
		case 4:
			r = t;
			g = p;
			b = v;
			break;
		default:		// case 5:
			r = v;
			g = p;
			b = q;
			break;
		}
	}
	
	
	ColourRGBA ColourRGBA::Lerp(const ColourRGBA& x, const ColourRGBA& y, float t)
	{
		ColourRGBA res;
		
		res.r = rush_lerp(x.r, y.r, t);
		res.g = rush_lerp(x.g, y.g, t);
		res.b = rush_lerp(x.b, y.b, t);
		res.a = rush_lerp(x.a, y.a, t);
		
		return res;
	}

	void ColourRGBA::apply_gamma( float x )
	{
		r = pow(r,x);
		g = pow(g,x);
		b = pow(b,x);
		a = pow(a,x);
	}


}

