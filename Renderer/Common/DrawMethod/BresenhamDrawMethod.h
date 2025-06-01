#pragma once
#include "DrawMethod.h"

namespace TerakoyaRenderer
{
	class BresenhamDrawMethod : public IDrawMethod
	{
	public:
		void Draw(const Vec2& v1, const Vec2& v2, const Vec2& v3, Image& image) override;

	private:
		void DrawBresenham(const Vec2& v1, const Vec2& v2, Image& image);
	};
}
