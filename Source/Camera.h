#pragma once

#include <cassert>
#include "Common.h"
#include "Vec3.h"

class Camera
{
public:
	double aspectRatio = 16.0 / 9.0;
	int32 imageWidth = 800;
	double vfov = 90.0;
	double focalLength = 1.0;

	void Init()
	{
		imageHeight = static_cast<int32>(static_cast<double>(imageWidth) / aspectRatio);
		imageHeight = (imageHeight < 1) ? 1 : imageHeight; // clamp

		const double fovTheta = DegreesToRadians(vfov);
		const double viewHeight = std::tan(fovTheta / 2.0);
		const double viewportHeight = 2.0 * viewHeight * focalLength;
		const double viewportWidth = viewportHeight * (static_cast<double>(imageWidth) / static_cast<double>(imageHeight));

		viewportU = Vec3{ viewportWidth, 0.0, 0.0 };
		viewportV = Vec3{ 0.0, -viewportHeight, 0.0 };

		// Space between pixels
		pixelDeltaU = viewportU / imageWidth;
		pixelDeltaV = viewportV / imageHeight;

		// Top left pixel location
		viewportTL = cameraPos - Vec3(0, 0, focalLength) - (viewportU / 2.0) - (viewportV / 2.0);
		pixel00Pos = viewportTL + (0.5 * (pixelDeltaU + pixelDeltaV));

		bInitialized = true;
	}

	constexpr Vec3 GetCameraPos() const 
	{ 
		assert(bInitialized);
		return cameraPos;
	}

	constexpr Vec3 ComputePixelCenter(double x, double y, Vec3 offset) const
	{
		assert(bInitialized);
		return pixel00Pos
			+ ((x + offset.x) * pixelDeltaU)
			+ ((y + offset.y) * pixelDeltaV);
	}

	constexpr int32 GetImageHeight() const { return imageHeight; }
	constexpr int32 GetImageWidth() const { return imageWidth; }

private:
	int32 imageHeight;

	// RHS coords (+Y = up, +X = right, +Z = depth)
	Vec3 cameraPos{ 0.0, 0.0, 0.0 };

	Vec3 viewportU; // Horiztonal viewport edge
	Vec3 viewportV; // Vertical viewport edge. Inverted so y = 0 is top left.

	// Space between pixels
	Vec3 pixelDeltaU;
	Vec3 pixelDeltaV;

	// Top left pixel location
	Vec3 viewportTL;
	Vec3 pixel00Pos;

	bool bInitialized = false;
};