#pragma once

#include "Common.h"
#include "Interval.h"
#include "Vec3.h"

#include <iostream>

using Color = Vec3;

constexpr void ColorToRGB(const Color& c, uint8& r, uint8& g, uint8& b)
{
	constexpr Interval intensity{ 0.0, 0.999 };

	// Scale 0-1 to 0-255 rgb range.
	r = static_cast<int32>(256 * intensity.Clamp(c.r));
	g = static_cast<int32>(256 * intensity.Clamp(c.g));
	b = static_cast<int32>(256 * intensity.Clamp(c.b));
}

inline void WriteColor(std::ostream& out, const Color& c)
{
	uint8 r, g, b;
	ColorToRGB(c, r, g, b);
	out << r << ' ' << g << ' ' << b;
}
