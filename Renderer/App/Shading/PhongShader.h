#pragma once
#include <Siv3D.hpp>
#include "../../Common/Shader/VertexShaderBase.h"
#include "../../Common/Shader/PixelShaderBase.h"

namespace TerakoyaRenderer::Shading
{
	struct PhongVertexInput : public ReceiveParameterBase
	{
		Vec3 position;
		Vec3 normal;
		Mat4x4 worldMatrix;
		Mat4x4 viewMatrix;
		Mat4x4 projectionMatrix;
		Mat4x4 viewportMatrix;
	};

	struct PhongVertexOutput : public ReceiveParameterBase
	{
		Vec4 worldPos;
		Vec3 worldNormal;
		Vec4 viewPos;
		Vec4 screenPos;
	};

	struct PhongPixelInput : public ReceiveParameterBase
	{
		Vector2D<int> screenPos;
		Vec3 directionalLight;
		Vec3 lightColor;
		Vec3 normal;
	};

	class PhongVertexShader : public VertexShaderBase
	{
	public:
		PhongVertexShader();

		virtual void Calculate() override;
	};

	class PhongPixelShader : public PixelShaderBase
	{
	public:
		PhongPixelShader();

		virtual void Calculate() override;
	};
}
