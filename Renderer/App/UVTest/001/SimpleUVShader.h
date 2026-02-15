#pragma once
#include <Siv3D.hpp>
#include "../../../Common/Shader/VertexShaderBase.h"
#include "../../../Common/Shader/PixelShaderBase.h"

namespace TerakoyaRenderer::UV
{
	struct SimpleUVVertexInput : public ReceiveParameterBase
	{
		Vec3 position;
		Vec3 normal;
		Vec2 uv;
		Mat4x4 worldMatrix;
		Mat4x4 viewMatrix;
		Mat4x4 projectionMatrix;
		Mat4x4 viewportMatrix;
	};

	struct SimpleUVVertexOutput : public ReceiveParameterBase
	{
		Vec4 worldPos;
		Vec3 worldNormal;
		Vec4 viewPos;
		Vec4 screenPos;
		Vec2 uv;
	};

	struct SimpleUVPixelInput : public ReceiveParameterBase
	{
		Vector2D<int> screenPos;
		Vec3 directionalLight;
		Vec3 lightColor;
		Vec3 normal;

		// Tex
		Vec2 uv;
		std::weak_ptr<Image> baseColorTexture;
	};

	class SimpleUVVertexShader : public VertexShaderBase
	{
	public:
		SimpleUVVertexShader();

		virtual void Calculate() override;
	};

	class SimpleUVPixelShader : public PixelShaderBase
	{
	public:
		SimpleUVPixelShader();

		virtual void Calculate() override;
	};
}
