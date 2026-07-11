#pragma once

#include "Common.h"

#include <cmath>
#include <iostream>

class Vec3
{
public:
	union
	{
		struct
		{
			double x;
			double y;
			double z;
		};
		struct
		{
			double r;
			double g;
			double b;
		};
		double components[3];
	};

	constexpr Vec3() : x(0.0), y(0.0), z(0.0) {}
	constexpr Vec3(double inX, double inY, double inZ)
		: x(inX)
		, y(inY)
		, z(inZ)
	{
	}

	constexpr Vec3 operator-() const { return { -x, -y, -z }; }
	constexpr double operator[](int index) const { return components[index]; }
	constexpr double& operator[](int index) { return components[index]; }

	constexpr Vec3& operator+=(const Vec3& rhs)
	{
		x += rhs.x;
		y += rhs.y;
		z += rhs.z;
		return *this;
	}

	constexpr Vec3& operator*=(const Vec3& rhs)
	{
		x *= rhs.x;
		y *= rhs.y;
		z *= rhs.z;
		return *this;
	}

	constexpr Vec3& operator*=(double s)
	{
		x *= s;
		y *= s;
		z *= s;
		return *this;
	}

	constexpr Vec3& operator/=(double t)
	{
		return *this *= 1.0 / t;
	}

	double Length() const
	{
		// std::sqrt not constexpr until c++26 :(
		return std::sqrt(LengthSquared());
	}

	constexpr double LengthSquared() const
	{
		return x * x + y * y + z * z;
	}

	bool IsNearZero() const
	{
		constexpr double threshold = 1e-8;
		return (std::fabs(x) < threshold) && (std::fabs(y) < threshold) && (std::fabs(z) < threshold);
	}

	static Vec3 Random(double min = 0.0, double max = 1.0)
	{
		return { RandomValue(min, max), RandomValue(min, max), RandomValue(min, max) };
	}

	static constexpr Vec3 Zero() { return Vec3{ 0,0,0 }; }
};

using Point3 = Vec3;

inline std::ostream& operator<<(std::ostream& out, const Vec3& vec)
{
	return out << vec.x << ' ' << vec.y << ' ' << vec.z;
}

constexpr Vec3 operator+(const Vec3& lhs, const Vec3& rhs)
{
	return { lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z };
}

constexpr Vec3 operator-(const Vec3& lhs, const Vec3& rhs)
{
	return { lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z };
}

constexpr Vec3 operator*(const Vec3& lhs, const Vec3& rhs)
{
	return { lhs.x * rhs.x, lhs.y * rhs.y, lhs.z * rhs.z };
}

constexpr Vec3 operator*(const Vec3& lhs, double s)
{
	return { lhs.x * s, lhs.y * s, lhs.z * s};
}

constexpr Vec3 operator*(double s, const Vec3& vec)
{
	return vec * s;
}

constexpr Vec3 operator/(const Vec3& vec, double s)
{
	return (1.0 / s) * vec;
}

constexpr double DotProduct(const Vec3& lhs, const Vec3& rhs)
{
	return lhs.x * rhs.x
		+ lhs.y * rhs.y
		+ lhs.z * rhs.z;
}

constexpr Vec3 CrossProduct(const Vec3& lhs, const Vec3& rhs)
{
	return
	{
		lhs.y * rhs.z - lhs.z * rhs.y,
		lhs.z * rhs.x - lhs.x * rhs.z,
		lhs.x * rhs.y - lhs.y * rhs.x
	};
}

constexpr double DistanceSq(const Vec3& a, const Vec3& b)
{
	return (a - b).LengthSquared();
}

inline double Distance(const Vec3& a, const Vec3& b)
{
	return (a - b).Length();
}

constexpr Vec3 Reflect(const Vec3& v, const Vec3& n)
{
	return v - 2 * DotProduct(v, n) * n;
}

inline Vec3 Refract(const Vec3& uv, const Vec3& n, double etaiOverEtat)
{
	const double cosTheta = std::fmin(DotProduct(-uv, n), 1.0);
	const Vec3 perpendicular = etaiOverEtat * (uv + cosTheta * n);
	const Vec3 parallel = -std::sqrt(std::fabs(1.0 - perpendicular.LengthSquared())) * n;
	return perpendicular + parallel;
}

constexpr Vec3 UnitVector(const Vec3& vec)
{
	return vec / vec.Length();
}

inline Vec3 RandomVectorInUnitSphere()
{
	while (true)
	{
		const Vec3 vec = Vec3::Random(-1.0, 1.0);
		const double lengthSq = vec.LengthSquared();
		// Inside the sphere if x^2 + y^2 + z^2 <= 1.
		if (lengthSq >= 1e-160 && lengthSq <= 1)
		{
			return vec / std::sqrt(lengthSq);
		}
	}
}

inline Vec3 RandomVectorOnHemisphere(const Vec3& normal)
{
	const Vec3 unitSphereVec = RandomVectorInUnitSphere();
	if (DotProduct(unitSphereVec, normal) > 0.0) // In the same hemisphere as the normal
	{
		return unitSphereVec;
	}
	else
	{
		return -unitSphereVec; // Invert to get onto same hemisphere.
	}
}

inline Vec3 RandomVectorInUnitDisk()
{
	while (true)
	{
		const Vec3 vec{ RandomValue(-1.0, 1.0), RandomValue(-1.0, 1.0), 0.0 };
		if (vec.LengthSquared() < 1)
		{
			return vec;
		}
	}
}

inline Vec3 GenerateSampleSquare()
{
	// Returns the vector to a random point in the [-.5,-.5]-[+.5,+.5] unit square.
	return Vec3{ RandomValue<double>(-0.5, 0.5), RandomValue<double>(-0.5, 0.5), 0 };
}
