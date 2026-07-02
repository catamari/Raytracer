#pragma once

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

	Vec3() : x(0.0), y(0.0), z(0.0) {}
	Vec3(double inX, double inY, double inZ)
		: x(inX)
		, y(inY)
		, z(inZ)
	{
	}

	Vec3 operator-() const { return { -x, -y, -z }; }
	double operator[](int index) const { return components[index]; }
	double& operator[](int index) { return components[index]; }

	Vec3& operator+=(const Vec3& rhs)
	{
		x += rhs.x;
		y += rhs.y;
		z += rhs.z;
		return *this;
	}

	Vec3& operator*=(const Vec3& rhs)
	{
		x *= rhs.x;
		y *= rhs.y;
		z *= rhs.z;
		return *this;
	}

	Vec3& operator*=(double s)
	{
		x *= s;
		y *= s;
		z *= s;
		return *this;
	}

	Vec3& operator/=(double t)
	{
		return *this *= 1.0 / t;
	}

	double Length() const
	{
		return std::sqrt(LengthSquared());
	}

	double LengthSquared() const
	{
		return x * x + y * y + z * z;
	}
};

using Point3 = Vec3;

inline std::ostream& operator<<(std::ostream& out, const Vec3& vec)
{
	return out << vec.x << ' ' << vec.y << ' ' << vec.z;
}

inline Vec3 operator+(const Vec3& lhs, const Vec3& rhs)
{
	return { lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z };
}

inline Vec3 operator-(const Vec3& lhs, const Vec3& rhs)
{
	return { lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z };
}

inline Vec3 operator*(const Vec3& lhs, const Vec3& rhs)
{
	return { lhs.x * rhs.x, lhs.y * rhs.y, lhs.z * rhs.z };
}

inline Vec3 operator*(const Vec3& lhs, double s)
{
	return { lhs.x * s, lhs.y * s, lhs.z * s};
}

inline Vec3 operator*(double s, const Vec3& vec)
{
	return vec * s;
}

inline Vec3 operator/(const Vec3& vec, double s)
{
	return (1.0 / s) * vec;
}

inline double DotProduct(const Vec3& lhs, const Vec3& rhs)
{
	return lhs.x * rhs.x
		+ lhs.y * rhs.y
		+ lhs.z + rhs.z;
}

inline Vec3 CrossProduct(const Vec3& lhs, const Vec3& rhs)
{
	return
	{
		lhs.y * rhs.z - lhs.z * rhs.y,
		lhs.z * rhs.x - lhs.x * rhs.z,
		lhs.x * rhs.y - lhs.y * rhs.x
	};
}

inline Vec3 UnitVector(const Vec3& vec)
{
	return vec / vec.Length();
}