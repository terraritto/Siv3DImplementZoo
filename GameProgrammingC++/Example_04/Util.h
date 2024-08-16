#pragma once
#include <Siv3D.hpp>

inline bool NearZero(float value, float epsilon = 0.001f)
{
	return fabs(value) <= epsilon;
}
