#pragma once
#include <Siv3D.hpp>
#include "../../Common/Shader/VertexShaderBase.h"
#include "../../Common/Shader/PixelShaderBase.h"

namespace TerakoyaRenderer::Shading
{
	struct GourandVertexInput : public ReceiveParameterBase
	{
		Vec3 position;
		Vec3 normal;
		Vec3 directionalLight;
		Vec3 lightColor;
		Mat4x4 worldMatrix;
		Mat4x4 viewMatrix;
		Mat4x4 projectionMatrix;
		Mat4x4 viewportMatrix;
	};

	struct GourandVertexOutput : public ReceiveParameterBase
	{
		Vec4 worldPos;
		Vec3 light;
		Vec4 viewPos;
		Vec4 screenPos;
	};

	struct GourandPixelInput : public ReceiveParameterBase
	{
		Vector2D<int> screenPos;
		Vec3 light;
	};

	class GourandVertexShader : public VertexShaderBase
	{
	public:
		GourandVertexShader();

		virtual void Calculate() override;
	};

	class GourandPixelShader : public PixelShaderBase
	{
	public:
		GourandPixelShader();

		virtual void Calculate() override;
	};
}
