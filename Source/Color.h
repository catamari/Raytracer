#pragma once

#include "Common.h"
#include "Vec3.h"

#include <iostream>

using Color = Vec3;

inline void ColorToRGB(const Color& c, uint8& r, uint8& g, uint8& b)
{
	// Scale 0-1 to 0-255 rgb range.
	r = static_cast<int32>(255.999 * c.r);
	g = static_cast<int32>(255.999 * c.g);
	b = static_cast<int32>(255.999 * c.b);
}

inline void WriteColor(std::ostream& out, const Color& c)
{
	uint8 r, g, b;
	ColorToRGB(c, r, g, b);
	out << r << ' ' << g << ' ' << b;
}
