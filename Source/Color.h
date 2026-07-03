#pragma once

#include "Common.h"
#include "Interval.h"
#include "Vec3.h"

#include <iostream>

using Color = Vec3;

inline double LinearToGamma(double linearComponent)
{
	return linearComponent > 0 ? std::sqrt(linearComponent) : 0.0; 
}

inline void ColorToRGB(const Color& c, uint8& r, uint8& g, uint8& b)
{
	constexpr Interval intensity{ 0.0, 0.999 };

	// Transform linear color into gamma space for color correction.
	const double rg = LinearToGamma(c.r);
	const double gg = LinearToGamma(c.g);
	const double bg = LinearToGamma(c.b);

	// Scale 0-1 to 0-255 rgb range.
	r = static_cast<int32>(256 * intensity.Clamp(rg));
	g = static_cast<int32>(256 * intensity.Clamp(gg));
	b = static_cast<int32>(256 * intensity.Clamp(bg));
}

inline void WriteColor(std::ostream& out, const Color& c)
{
	uint8 r, g, b;
	ColorToRGB(c, r, g, b);
	out << r << ' ' << g << ' ' << b;
}
