#pragma once
#include <Siv3D.hpp>

namespace TerakoyaRenderer
{
	enum class EDrawMethod
	{
		Point,
		Line,
		Fill
	};

	class IDrawMethod
	{
	public:
		virtual void Draw(const Vec2& v1, const Vec2& v2, const Vec2& v3, Image& image) = 0;
	};
}
