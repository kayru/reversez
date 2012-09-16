#include "MathVector3.h"
#include <math.h>

namespace Rush
{
	float Vector3::length() const
	{
		return sqrt(length_squared());
	}
}
