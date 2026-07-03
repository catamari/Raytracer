#include <cstdio>
#include <cstring>
#include <optional>
#include <iostream>
#include <vector>

#include "Common.h"
#include "Color.h"
#include "Image.h"
#include "Vec3.h"
#include "Ray.h"

enum class ShapeType : uint8
{
	Sphere
};

struct Shape
{
	ShapeType type;
	Point3 center;
	double radius;
};

struct HitRecord
{
	Point3 point;
	Vec3 normal;
	double t;
	bool isFrontFace;
};

struct World
{
	std::vector<Shape> shapes;
};

bool DoesRayHitShape(const Ray& ray, const Shape& shape, double tMin, double tMax, HitRecord& outHitRecord)
{
	if (shape.type == ShapeType::Sphere)
	{
		const Vec3 rayToSphere = shape.center - ray.Origin();
		const double a = ray.Direction().LengthSquared();
		const double h = DotProduct(ray.Direction(), rayToSphere);
		const double c = rayToSphere.LengthSquared() - (shape.radius * shape.radius);
		const double discriminant = h * h - a * c;
		if (discriminant < 0)
		{
			return false;
		}

		const auto CalcRoot = [&]() -> std::optional<double>
		{
			const double sqrtd = std::sqrt(discriminant);
			const double root1 = (h - sqrtd) / a;
			const double root2 = (h + sqrtd) / a;
			if (IsValueInRange(root1, tMin, tMax))
			{
				return root1;
			}
			else if (IsValueInRange(root2, tMin, tMax))
			{
				return root2;
			}
			else
			{
				return std::nullopt;
			}
		};

		const std::optional<double> root = CalcRoot();
		if (!root.has_value())
		{
			return false;
		}

		const double t = root.value();
		const Point3 hitPoint = ray.At(root.value());
		const Vec3 outwardNormal = (hitPoint - shape.center) / shape.radius;
		const bool frontFace = DotProduct(ray.Direction(), outwardNormal) < 0;

		outHitRecord.t = t;
		outHitRecord.point = hitPoint;
		outHitRecord.normal = frontFace ? outwardNormal : -outwardNormal;
		outHitRecord.isFrontFace = frontFace;
		return true;
	}
	return false;
}

bool GetFirstRayHit(const Ray& ray, const World& world, double tMin, double tMax, HitRecord& firstHit)
{
	double closestHit = tMax;
	bool anyHits = false;
	for (const Shape& shape : world.shapes)
	{
		if (DoesRayHitShape(ray, shape, tMin, closestHit, firstHit))
		{
			anyHits = true;
			closestHit = firstHit.t;
		}
	}
	return anyHits;
}

Color CalculateRayColor(const Ray& ray, const World& world)
{
	// Hard coded sphere for now
	HitRecord hit;
	if (GetFirstRayHit(ray, world, 0, infinity, hit))
	{
		// Colorize each point based on the normals
		return 0.5 * (hit.normal + Color(1.0, 1.0, 1.0));
	}

	const Vec3 UnitDir = UnitVector(ray.Direction());
	const double a = 0.5 * (UnitDir.y + 1.0);
	return (1.0 - a) * Color(1.0, 1.0, 1.0) + a * (Color(0.5, 0.7, 1.0));
}

int main()
{
	// Image
	const double aspectRatio = 16.0 / 9.0;
	const int32 imageWidth = 400;
	int32 imageHeight = static_cast<int32>(static_cast<double>(imageWidth) / aspectRatio);
	imageHeight = (imageHeight < 1) ? 1 : imageHeight; // clamp

	World world;
	world.shapes.push_back(Shape{ .type = ShapeType::Sphere, .center = Point3{0,0,-1}, .radius = 0.5 });
	world.shapes.push_back(Shape{ .type = ShapeType::Sphere, .center = Point3{0,-100.5,-1}, .radius = 100 });

	// Camera
	const double focalLength = 1.0;
	const double viewportHeight = 2.0;
	const double viewportWidth = viewportHeight * (static_cast<double>(imageWidth) / static_cast<double>(imageHeight));
	// RHS coords (+Y = up, +X = right, +Z = depth)
	const Vec3 cameraPos{0.0, 0.0, 0.0};

	const Vec3 viewportU{ viewportWidth, 0.0, 0.0 }; // Horiztonal viewport edge
	const Vec3 viewportV{ 0.0, -viewportHeight, 0.0 }; // Vertical viewport edge. Inverted so y = 0 is top left.

	// Space between pixels
	const Vec3 pixelDeltaU = viewportU / imageWidth;
	const Vec3 pixelDeltaV = viewportV / imageHeight;

	// Top left pixel location
	const Vec3 viewportTL = cameraPos - Vec3(0, 0, focalLength) - (viewportU / 2.0) - (viewportV / 2.0);
	const Vec3 pixel00Pos = viewportTL + (0.5 * (pixelDeltaU + pixelDeltaV));

	printf("Aspect ratio: %.2f | Image W: %d H: %d | Viewport H: %.2f W: %.2f", aspectRatio, imageWidth, imageHeight, viewportWidth, viewportHeight);

	// Render
	const uint32 buffSize = imageWidth * imageHeight;
	uint8* buffer = new uint8[buffSize * 3];
	uint32 writeIndex = 0;
	for (int32 y = 0; y < imageHeight; ++y)
	{
		for (int32 x = 0; x < imageWidth; ++x)
		{
			const Vec3 pixelCenter = pixel00Pos + (x * pixelDeltaU) + (y * pixelDeltaV);
			const Vec3 rayDir = pixelCenter - cameraPos;
			Ray ray{ cameraPos, rayDir };

			Color pixelColor = CalculateRayColor(ray, world);

			const int32 index = (y * imageWidth) + x;
			ColorToRGB(pixelColor, buffer[writeIndex], buffer[writeIndex + 1], buffer[writeIndex + 2]);
			writeIndex += 3;
		}
	}

	BufferToPPM("Frame.ppm", imageWidth, imageHeight, buffer);
	delete[] buffer;

	return 0;
}