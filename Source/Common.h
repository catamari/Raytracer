#pragma once

#include <limits>
#include <cstdlib>
#include <random>
#include <concepts>

using int8 = char;
using int16 = short;
using int32 = int;
using int64 = long long;
using uint8 = unsigned char;
using uint16 = unsigned short;
using uint32 = unsigned int;
using uint64 = unsigned long long;

constexpr double infinity = std::numeric_limits<double>::infinity();
constexpr double pi = 3.1415926535897932385;

constexpr double DegreesToRadians(double degrees) 
{
    return degrees * pi / 180.0;
}

template<bool Inclusive = true>
constexpr bool IsValueInRange(double value, double min, double max)
{
	if constexpr (Inclusive)
	{
		return value >= min && value <= max;
	}
	else
	{
		return value > min && value < max;
	}
}

template<typename T = double>
inline T RandomValue(T min = T(0), T max = T(1))
{
	static std::uniform_real_distribution<T> distribution{ min, max };
	static std::mt19937 generator;
	return distribution(generator);
}
