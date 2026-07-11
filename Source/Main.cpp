#include <cstdio>
#include <cstring>
#include <optional>
#include <iostream>
#include <vector>

#include "Camera.h"
#include "Common.h"
#include "Color.h"
#include "HitRecord.h"
#include "Image.h"
#include "Interval.h"
#include "Material.h"
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

	const Material* material = nullptr;
};

struct World
{
	std::vector<Shape> shapes;
};

bool DoesRayHitShape(const Ray& ray, const Shape& shape, Interval interval, HitRecord& outHitRecord)
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
			if (interval.Contains(root1))
			{
				return root1;
			}
			else if (interval.Contains(root2))
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
		outHitRecord.material = shape.material;
		return true;
	}
	return false;
}

bool GetFirstRayHit(const Ray& ray, const World& world, Interval interval, HitRecord& firstHit)
{
	double closestHit = interval.max;
	bool anyHits = false;
	for (const Shape& shape : world.shapes)
	{
		if (DoesRayHitShape(ray, shape, Interval{ interval.min, closestHit }, firstHit))
		{
			anyHits = true;
			closestHit = firstHit.t;
		}
	}
	return anyHits;
}

Color CalculateRayColor(const Ray& ray, const World& world, int32 depth)
{
	if (depth <= 0)
	{
		return Color{ 0,0,0 };
	}

	HitRecord hit;
	// Hack: add slight min bias to prevent re-intersecting with the same surface due to floating point error (shadow acne).
	if (GetFirstRayHit(ray, world, Interval{ 0.001, infinity }, hit))
	{
		Ray scattered;
		Color attenuation;
		if (hit.material && Scatter(*hit.material, ray, hit, attenuation, scattered))
		{
			return attenuation * CalculateRayColor(scattered, world, depth - 1);
		}
		return Color(0, 0, 0);
	}

	const Vec3 UnitDir = UnitVector(ray.Direction());
	const double a = 0.5 * (UnitDir.y + 1.0);
	return (1.0 - a) * Color(1.0, 1.0, 1.0) + a * (Color(0.5, 0.7, 1.0));
}

int main()
{
	//const Material mat_ground
	//{
	//	.type = MaterialType::Lambert,
	//	.albedo = Color(0.8, 0.8, 0.0)
	//};
	//const Material mat_centerSphere
	//{
	//	.type = MaterialType::Lambert,
	//	.albedo = Color(0.1, 0.2, 0.5)
	//};
	//const Material mat_leftSphere
	//{
	//	.type = MaterialType::Dielectric,
	//	.refractiveIndex = 1.5
	//};
	//const Material mat_leftBubble
	//{
	//	.type = MaterialType::Dielectric,
	//	.refractiveIndex = 1.0 / 1.5
	//};
	//const Material mat_rightSphere
	//{
	//	.type = MaterialType::Metal,
	//	.albedo = Color(0.8, 0.6, 0.2),
	//	.fuzz = 1.0
	//};

	//World world;
	//world.shapes.push_back(Shape{ .type = ShapeType::Sphere, .center = Point3{ 0,	-100.5,	-1}, .radius = 100, .material = &mat_ground });
	//world.shapes.push_back(Shape{ .type = ShapeType::Sphere, .center = Point3{ 0,	0,		-1.2}, .radius = 0.5, .material = &mat_centerSphere });
	//world.shapes.push_back(Shape{ .type = ShapeType::Sphere, .center = Point3{-1,	0,		-1.0}, .radius = 0.5, .material = &mat_leftSphere });
	//world.shapes.push_back(Shape{ .type = ShapeType::Sphere, .center = Point3{-1,	0,		-1.0}, .radius = 0.4, .material = &mat_leftBubble });
	//world.shapes.push_back(Shape{ .type = ShapeType::Sphere, .center = Point3{ 1,	0,		-1.0}, .radius = 0.5, .material = &mat_rightSphere });

	std::vector<std::unique_ptr<Material>> materals;

	World world;

	const Material mat_ground
	{
		.type = MaterialType::Lambert,
		.albedo = Color(0.5, 0.5, 0.5)
	};

	world.shapes.push_back(Shape{ .type = ShapeType::Sphere, .center = Point3{ 0,	-1000,	0}, .radius = 1000, .material = &mat_ground });

	for (int32 i = -11; i < 11; ++i)
	{
		for (int32 j = -11; j < 11; ++j)
		{
			const double randomMatRoll = RandomValue();
			const Point3 center{ i + 0.9 * RandomValue(), 0.2, j + 0.9 * RandomValue() };

			if (Distance(center, Point3{ 4, 0.2, 0 }) > 0.9)
			{
				std::unique_ptr<Material> mat;
				Shape shape;
				if (randomMatRoll < 0.8)
				{
					mat = std::make_unique<Material>(Material::MakeLambert(Color::Random() * Color::Random()));
					shape = Shape{ .type = ShapeType::Sphere, .center = center, .radius = 0.2, .material = mat.get()};
				}
				else if (randomMatRoll < 0.95)
				{
					mat = std::make_unique<Material>(Material::MakeMetal(Color::Random(0.5, 1.0), RandomValue(0.0, 0.5)));
					shape = Shape{ .type = ShapeType::Sphere, .center = center, .radius = 0.2, .material = mat.get() };
				}
				else
				{
					mat = std::make_unique<Material>(Material::MakeDielectric(1.5)); // glass
					shape = Shape{ .type = ShapeType::Sphere, .center = center, .radius = 0.2, .material = mat.get() };
				}

				world.shapes.push_back(std::move(shape));
				materals.push_back(std::move(mat));
			}
		}
	}

	const Material material1 = Material::MakeDielectric(1.5);
	world.shapes.push_back(Shape{ .type = ShapeType::Sphere, .center{0, 1, 0}, .radius = 1.0, .material = &material1 });

	const Material material2 = Material::MakeLambert(Color(0.4, 0.2, 0.1));
	world.shapes.push_back(Shape{ .type = ShapeType::Sphere, .center{-4, 1, 0}, .radius = 1.0, .material = &material2 });

	const Material material3 = Material::MakeMetal(Color(0.7, 0.6, 0.5), 0.0);
	world.shapes.push_back(Shape{ .type = ShapeType::Sphere, .center{4, 1, 0}, .radius = 1.0, .material = &material3 });

	Camera camera;
	camera.aspectRatio = 16.0 / 9.0;
	camera.imageWidth = 1200;
	camera.vfov = 20.0;
	camera.cameraPos = Vec3{ 13, 2, 3 };
	camera.lookAt = Vec3{ 0, 0, 0 };
	camera.defocusAngleDeg = 0.6;
	camera.focusDistance = 10.0;
	camera.Init();

	// Render
	const int32 samplesPerPixel = 500;
	const double pixelSampleScale = 1.0 / (double)samplesPerPixel;

	constexpr int32 maxBounces = 50;

	const int32 imageHeight = camera.GetImageHeight();
	const int32 imageWidth = camera.GetImageWidth();
	const uint32 buffSize = imageHeight * imageWidth * 3;
	uint8* buffer = new uint8[buffSize];
	uint32 writeIndex = 0;
	for (int32 y = 0; y < imageHeight; ++y)
	{
		for (int32 x = 0; x < imageWidth; ++x)
		{
			Color pixelColor{0,0,0};
			for (int32 sample = 0; sample < samplesPerPixel; ++sample)
			{
				const Ray ray = camera.ComputeRay(x, y);
				pixelColor += CalculateRayColor(ray, world, maxBounces);
			}

			pixelColor *= pixelSampleScale;

			const int32 index = (y * imageWidth) + x;
			assert(writeIndex >= 0 && (writeIndex + 2) < buffSize);
			ColorToRGB(pixelColor, buffer[writeIndex], buffer[writeIndex + 1], buffer[writeIndex + 2]);
			writeIndex += 3;
		}
	}

	BufferToPPM("Frame.ppm", imageWidth, imageHeight, buffer);
	delete[] buffer;

	return 0;
}