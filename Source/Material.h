#pragma once

#include "Color.h"
#include "Vec3.h"

class Ray;
struct HitRecord;

enum class MaterialType : uint8
{
	Lambert,
	Metal,
	Dielectric
};

struct Material
{
	MaterialType type;
	Color albedo;
	// Reflection fuzz factor.
	double fuzz = 1.0;
	// Refractive index in vacuum or air, or the ratio of the material's refractive index over
	// the refractive index of the enclosing media
	double refractiveIndex = 1.0;

	static Material MakeLambert(const Color& albedo);
	static Material MakeMetal(const Color& albedo, double fuzz);
	static Material MakeDielectric(double refractiveIndex);
};

bool Scatter(const Material& mat, const Ray& ray, const HitRecord& hit, Color& outAttenuation, Ray& outScattered);
