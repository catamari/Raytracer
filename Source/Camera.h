#pragma once

#include <cassert>
#include "Common.h"
#include "Ray.h"
#include "Vec3.h"

class Camera
{
public:
	double aspectRatio = 16.0 / 9.0;
	double vfov = 90.0;
	double defocusAngleDeg = 0.0; // Variation angle of rays through each pixel
	double focusDistance = 10.0; // Distance from camera pos to plane of perfect focus.
	int32 imageWidth = 800;

	// RHS coords (+Y = up, +X = right, +Z = depth)
	Point3 cameraPos = Point3::Zero(); // look from
	Point3 lookAt = Point3::Zero();
	Vec3 upDir = Vec3{ 0, 1, 0 };

	void Init()
	{
		imageHeight = static_cast<int32>(static_cast<double>(imageWidth) / aspectRatio);
		imageHeight = (imageHeight < 1) ? 1 : imageHeight; // clamp

		const double fovTheta = DegreesToRadians(vfov);
		const double viewHeight = std::tan(fovTheta / 2.0);
		const double viewportHeight = 2.0 * viewHeight * focusDistance;
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
		const Vec3 viewportTL = cameraPos - (focusDistance * w) - (viewportU / 2.0) - (viewportV / 2.0);
		pixel00Pos = viewportTL + (0.5 * (pixelDeltaU + pixelDeltaV));

		const double defocusRadius = focusDistance * std::tan(DegreesToRadians(defocusAngleDeg / 2.0));
		defocusDiskU = u * defocusRadius;
		defocusDiskV = v * defocusRadius;

		bInitialized = true;
	}

	Ray ComputeRay(double x, double y) const
	{
		assert(bInitialized);

		// Construct a camera ray directed at a randomly sampled point around the pixel location x,y.
		const Vec3 offset = GenerateSampleSquare();
		const Vec3 pixelCenter = ComputePixelCenter(x, y, offset);
		const Vec3 rayOrigin = (defocusAngleDeg <= 0) ? cameraPos : ComputeDefocusDiskSample();
		const Vec3 rayDir = pixelCenter - rayOrigin;
		return Ray{ rayOrigin, rayDir };
	}

	constexpr Vec3 GetCameraPos() const 
	{ 
		assert(bInitialized);
		return cameraPos;
	}

	constexpr int32 GetImageHeight() const { return imageHeight; }
	constexpr int32 GetImageWidth() const { return imageWidth; }

private:
	constexpr Vec3 ComputePixelCenter(double x, double y, Vec3 offset) const
	{
		assert(bInitialized);
		return pixel00Pos
			+ ((x + offset.x) * pixelDeltaU)
			+ ((y + offset.y) * pixelDeltaV);
	}

	Vec3 ComputeDefocusDiskSample() const
	{
		const Vec3 p = RandomVectorInUnitDisk();
		return cameraPos + (p.x * defocusDiskU) + (p.y * defocusDiskV);
	}

	Vec3 u, v, w; // Camera frame basis vectors
	Vec3 defocusDiskU; // Defocus disk horizontal radius
	Vec3 defocusDiskV; // Defocus disk vertical radius

	// Space between pixels
	Vec3 pixelDeltaU;
	Vec3 pixelDeltaV;

	// Top left pixel location
	Vec3 pixel00Pos;

	int32 imageHeight;

	bool bInitialized = false;
};