#pragma once
#include <Siv3D.hpp>
#include "ShaderBase.h"

namespace TerakoyaRenderer
{
	class PixelShaderBase : public ShaderBase
	{
	public:
		// 描画を行う画像を設定
		void SetImage(Image* image) { m_image = image; }

	protected:
		Image* m_image;
	};
}
