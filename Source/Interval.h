#pragma once

#include "Common.h"

struct Interval
{
	double min = +infinity;
	double max = -infinity;

	constexpr Interval() = default;
	constexpr Interval(double inMin, double inMax)
		: min(inMin)
		, max(inMax)
	{
	}

	constexpr double Size() const
	{
		return max - min;
	}

	constexpr bool Contains(double value) const
	{
		return IsValueInRange<true>(value, min, max);
	}

	constexpr bool Surrounds(double value) const
	{
		return IsValueInRange<false>(value, min, max);
	}

	constexpr double Clamp(double value) const
	{
		if (value < min) return min;
		if (value > max) return max;
		return value;
	}

	static constexpr Interval Empty()
	{
		return {};
	}

	static constexpr Interval Universe()
	{
		return { -infinity, +infinity };
	}
};
