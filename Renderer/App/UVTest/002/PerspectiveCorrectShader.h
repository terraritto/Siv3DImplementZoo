#pragma once
#include <Siv3D.hpp>
#include "../../../Common/Shader/VertexShaderBase.h"
#include "../../../Common/Shader/PixelShaderBase.h"

namespace TerakoyaRenderer::UV
{
	struct PerspectiveCorrectVertexInput : public ReceiveParameterBase
	{
		Vec3 position;
		Vec3 normal;
		Vec2 uv;
		Mat4x4 worldMatrix;
		Mat4x4 viewMatrix;
		Mat4x4 projectionMatrix;
		Mat4x4 viewportMatrix;
	};

	struct PerspectiveCorrectVertexOutput : public ReceiveParameterBase
	{
		Vec4 worldPos;
		Vec3 worldNormal;
		Vec4 viewPos;
		Vec4 screenPos;
		Vec2 uv;
	};

	struct PerspectiveCorrectPixelInput : public ReceiveParameterBase
	{
		Vector2D<int> screenPos;
		Vec3 directionalLight;
		Vec3 lightColor;
		Vec3 normal;

		// Tex
		Vec2 uv;
		std::weak_ptr<Image> baseColorTexture;
	};

	class PerspectiveCorrectVertexShader : public VertexShaderBase
	{
	public:
		PerspectiveCorrectVertexShader();

		virtual void Calculate() override;
	};

	class PerspectiveCorrectPixelShader : public PixelShaderBase
	{
	public:
		PerspectiveCorrectPixelShader();

		virtual void Calculate() override;
	};
}
