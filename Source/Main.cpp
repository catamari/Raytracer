#include <cstdio>
#include <cstring>
#include <iostream>

#include "Common.h"
#include "Color.h"
#include "Vec3.h"
#include "Ray.h"

void BufferToPPM(const char* filename, uint32 width, uint32 height, const uint8* buffer)
{
	FILE* f = nullptr;
	if (fopen_s(&f, filename, "w") == 0)
	{
		fprintf_s(f, "P3\n");
		fprintf_s(f, "%lu %lu\n255\n", width, height);

		const uint32 size = width * height * 3;
		for (uint32 i = 0; i < size; i += 3)
		{
			//std::clog << "\rScanlines remaining: " << i / 3 / width << std::flush;
			fprintf_s(f, "%u %u %u\n", buffer[i], buffer[i + 1], buffer[i + 2]);
		}

		fclose(f);
	}
	std::clog << "\rDone.                          \n";
}

void GenerateTestImage()
{
	const uint32 width = 256;
	const uint32 height = 256;
	const uint32 size = width * height;
	uint8* buffer = new uint8[size * 3];
	uint32 writeIndex = 0;
	for (uint32 y = 0; y < height; ++y)
	{
		for (uint32 x = 0; x < width; ++x)
		{
			const double r = static_cast<double>( x ) / static_cast<double>( width - 1 );
			const double g = static_cast<double>( y ) / static_cast<double>( height - 1 );
			const double b = 0.0;
			const Color c{ r, g, b };

			const uint32 index = (y * width) + x;
			ColorToRGB(c, buffer[writeIndex], buffer[writeIndex+1], buffer[writeIndex+2]);
			writeIndex += 3;
		}
	}

	BufferToPPM("Gradient.ppm", width, height, buffer);
	delete[] buffer;
}

Color CalculateRayColor(const Ray& ray)
{
	const Vec3 UnitDir = UnitVector(ray.Direction());
	const double a = 0.5 * (UnitDir.y + 1.0);
	return (1.0 - a) * Color(1.0, 1.0, 1.0) + a * (Color(0.5, 0.7, 1.0));
}

int main()
{
	//GenerateTestImage();

	// Image
	const double aspectRatio = 16.0 / 9.0;
	const int32 imageWidth = 400;
	int32 imageHeight = static_cast<int32>(static_cast<double>(imageWidth) / aspectRatio);
	imageHeight = (imageHeight < 1) ? 1 : imageHeight; // clamp


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
	for (uint32 y = 0; y < imageHeight; ++y)
	{
		for (uint32 x = 0; x < imageWidth; ++x)
		{
			const Vec3 pixelCenter = pixel00Pos + (x * pixelDeltaU) + (y * pixelDeltaV);
			const Vec3 rayDir = pixelCenter - cameraPos;
			Ray ray{ cameraPos, rayDir };

			Color pixelColor = CalculateRayColor(ray);

			const uint32 index = (y * imageWidth) + x;
			ColorToRGB(pixelColor, buffer[writeIndex], buffer[writeIndex + 1], buffer[writeIndex + 2]);
			writeIndex += 3;
		}
	}

	BufferToPPM("Frame.ppm", imageWidth, imageHeight, buffer);
	delete[] buffer;

	return 0;
}