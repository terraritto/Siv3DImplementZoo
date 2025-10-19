#pragma once
#include "DrawMethod.h"

namespace TerakoyaRenderer
{
	class FillDrawMethod : public IDrawMethod
	{
	public:
		void Draw(const Vec2& v1, const Vec2& v2, const Vec2& v3, Image& image) override;

	public:
		std::weak_ptr<ShaderBase> m_pixelShader;
	};
}
