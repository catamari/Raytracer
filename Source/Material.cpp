#include "Material.h"

#include "HitRecord.h"
#include "Ray.h"

bool ScatterLambert(const Material& mat, const Ray& ray, const HitRecord& hit, Color& outAttenuation, Ray& outScattered)
{
	// Lambertian reflection - reflected rays are more likely to scatter in a direction near the surface normal.
	// By offsetting by the hit normal, we're forcing the random unit sphere vector to be within the unit sphere on the outside of the shape.
	Vec3 scatterDirection = hit.normal + RandomVectorInUnitSphere();

	// Handle degenerate scatter directions.
	if (scatterDirection.IsNearZero())
	{
		scatterDirection = hit.normal;
	}

	outScattered = Ray{ hit.point, scatterDirection };
	outAttenuation = mat.albedo;
	return true;
}

bool ScatterMetal(const Material& mat, const Ray& ray, const HitRecord& hit, Color& outAttenuation, Ray& outScattered)
{
	const double fuzz = mat.fuzz < 1.0 ? mat.fuzz : 1.0;
	const Vec3 reflected = Reflect(ray.Direction(), hit.normal);
	const Vec3 scattered = UnitVector(reflected) + (RandomVectorInUnitSphere() * fuzz);
	outScattered = Ray(hit.point, scattered);
	outAttenuation = mat.albedo;
	return DotProduct(scattered, hit.normal) > 0.0;
}

bool ScatterDielectric(const Material& mat, const Ray& ray, const HitRecord& hit, Color& outAttenuation, Ray& outScattered)
{
	outAttenuation = Color(1.0, 1.0, 1.0);
	const double refractiveIndex = hit.isFrontFace ? (1.0 / mat.refractiveIndex) : mat.refractiveIndex;

	const Vec3 unitDirection = UnitVector(ray.Direction());
	const double cosTheta = std::fmin(DotProduct(-unitDirection, hit.normal), 1.0);
	const double sinTheta = std::sqrt(1.0 - cosTheta * cosTheta); // trigonometric identity
	const bool canRefract = (refractiveIndex * sinTheta) < 1.0;
	Vec3 direction;
	if (canRefract)
	{
		direction = Refract(unitDirection, hit.normal, refractiveIndex);
	}
	else
	{
		// Total internal reflection
		direction = Reflect(unitDirection, hit.normal);
	}

	outScattered = Ray{ hit.point, direction };
	return true;
}

bool Scatter(const Material& mat, const Ray& ray, const HitRecord& hit, Color& outAttenuation, Ray& outScattered)
{
	switch (mat.type)
	{
	case MaterialType::Lambert:
		return ScatterLambert(mat, ray, hit, outAttenuation, outScattered);

	case MaterialType::Metal:
		return ScatterMetal(mat, ray, hit, outAttenuation, outScattered);

	case MaterialType::Dielectric:
		return ScatterDielectric(mat, ray, hit, outAttenuation, outScattered);
	}
	return false;
}