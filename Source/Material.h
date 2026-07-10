#pragma once

#include "Color.h"
#include "Vec3.h"

class Ray;
struct HitRecord;

enum class MaterialType : uint8
{
	Lambert,
	Metal
};

struct Material
{
	MaterialType type;
	Color albedo;
	double fuzz = 1.0;
};

bool Scatter(const Material& mat, const Ray& ray, const HitRecord& hit, Color& outAttenuation, Ray& outScattered);
