#pragma once
#include <limits>

int square(const int value)
{
	return value * value;
}

int compareDoubles(double a, double b) {
	return (a == b || std::abs(a - b) < std::abs(std::min(a, b))*std::numeric_limits<double>::epsilon());
}