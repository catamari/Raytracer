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

bool Scatter(const Material& mat, const Ray& ray, const HitRecord& hit, Color& outAttenuation, Ray& outScattered)
{
	switch (mat.type)
	{
	case MaterialType::Lambert:
		return ScatterLambert(mat, ray, hit, outAttenuation, outScattered);
	}
	return false;
}