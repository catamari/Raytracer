#pragma once

#include <cassert>
#include "Common.h"
#include "Vec3.h"

class Camera
{
public:
	double aspectRatio = 16.0 / 9.0;
	double vfov = 90.0;
	int32 imageWidth = 800;

	// RHS coords (+Y = up, +X = right, +Z = depth)
	Point3 cameraPos = Point3::Zero(); // look from
	Point3 lookAt = Point3::Zero();
	Vec3 upDir = Vec3{ 0, 1, 0 };

	void Init()
	{
		imageHeight = static_cast<int32>(static_cast<double>(imageWidth) / aspectRatio);
		imageHeight = (imageHeight < 1) ? 1 : imageHeight; // clamp

		double focalLength = (cameraPos - lookAt).Length();

		const double fovTheta = DegreesToRadians(vfov);
		const double viewHeight = std::tan(fovTheta / 2.0);
		const double viewportHeight = 2.0 * viewHeight * focalLength;
		const double viewportWidth = viewportHeight * (static_cast<double>(imageWidth) / static_cast<double>(imageHeight));

		// Calc camera basis vectors for camera coordinate frame
		w = UnitVector(cameraPos - lookAt); // backwards
		u = UnitVector(CrossProduct(upDir, w)); // right
		v = CrossProduct(w, u); // up

		const Vec3 viewportU = viewportWidth * u; // Vector across viewport horizontal edge
		const Vec3 viewportV = viewportHeight * -v; // Vector across viewport vertical edge

		// Space between pixels
		pixelDeltaU = viewportU / imageWidth;
		pixelDeltaV = viewportV / imageHeight;

		// Top left pixel location
		const Vec3 viewportTL = cameraPos - (focalLength * w) - (viewportU / 2.0) - (viewportV / 2.0);
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
	Vec3 u, v, w; // Camera frame basis vectors

	// Space between pixels
	Vec3 pixelDeltaU;
	Vec3 pixelDeltaV;

	// Top left pixel location
	Vec3 pixel00Pos;

	int32 imageHeight;

	bool bInitialized = false;
};