#include "MathVector4.h"

#include <math.h>

namespace Rush
{
	float Vector4::length() const
	{
		return sqrt(length_squared());
	}
}
