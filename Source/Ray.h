#pragma once

#include "Common.h"
#include "Vec3.h"

class Ray
{
public:
	Ray() = default;
	Ray(const Point3& inOrigin, const Vec3& inDirection)
		: origin(inOrigin)
		, direction(inDirection)
	{}

	const Point3& Origin() const { return origin; }
	const Vec3& Direction() const { return direction; }

	Point3 At(double t) const
	{
		return origin + (direction * t);
	}

private:
	Point3 origin;
	Vec3 direction;
};
