#include "MathVector2.h"
#include <math.h>

namespace Rush
{
	float Vector2::length() const
	{
		return sqrt(length_squared());
	}
}
