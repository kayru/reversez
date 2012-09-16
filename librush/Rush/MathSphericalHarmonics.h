#pragma once

#include "Rush.h"

#include "MathPixel.h"
#include "MathVector3.h"

namespace Rush
{
	/// @addtogroup math
	/// @{

	/// Third order (9-coefficient) SH, as per "Spherical Harmonics in Actual Games" presentation
	/// and http://graphics.stanford.edu/papers/envmap/envmap.pdf
	/// and http://www.cs.columbia.edu/~cs4162/slides/spherical-harmonic-lighting.pdf
	class SphericalHarmonics3
	{
	public:

		SphericalHarmonics3();
		SphericalHarmonics3(float* r, float* g, float* b);

		void		add(const Vector3& colour);
		void		add(const SphericalHarmonics3& rhs);
		void		add(const Vector3& dir, const Vector3& colour);
		void		mul(float s);
		Vector4		evaluate(const Vector3& dir);
		void		lerp(const SphericalHarmonics3& target, float a);

		static void	weights(float* out, const Vector3& dir);
		
		void generate_constants_opt(Vector4* out) const;
		void generate_constants(Vector4* out) const;

	private:

		Vector4 m_coefficients[9];

	};

	/// @}
}

