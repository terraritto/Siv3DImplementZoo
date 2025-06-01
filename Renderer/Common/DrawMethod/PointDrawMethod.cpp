#include "PointDrawMethod.h"

namespace TerakoyaRenderer
{
	void PointDrawMethod::Draw(const Vec2& v1, const Vec2& v2, const Vec2& v3, Image& image)
	{
		image[static_cast<int>(v1.y)][static_cast<int>(v1.x)] = Palette::White;
		image[static_cast<int>(v2.y)][static_cast<int>(v2.x)] = Palette::White;
		image[static_cast<int>(v3.y)][static_cast<int>(v3.x)] = Palette::White;
	}
}
