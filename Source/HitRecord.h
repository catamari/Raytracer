#pragma once

#include "Vec3.h"

struct Material;

struct HitRecord
{
	Point3 point;
	Vec3 normal;
	double t;
	const Material* material = nullptr;
	bool isFrontFace;
};