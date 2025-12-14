#pragma once
#include <Siv3D.hpp>
#include "../../Common/Shader/VertexShaderBase.h"
#include "../../Common/Shader/PixelShaderBase.h"

namespace TerakoyaRenderer::RemovalFace
{
	struct RemovalFaceVertexInput : public ReceiveParameterBase
	{
		Vec3 position;
		Mat4x4 worldMatrix;
		Mat4x4 viewMatrix;
		Mat4x4 projectionMatrix;
		Mat4x4 viewportMatrix;
	};

	struct RemovalFaceVertexOutput : public ReceiveParameterBase
	{
		Vec4 worldPos;
		Vec4 viewPos;
		Vec4 screenPos;
	};

	struct RemovalFacePixelInput : public ReceiveParameterBase
	{
		Vector2D<int> screenPos;
		Vec3 normal;
		Vec3 directionalLight;
		Vec3 lightColor;
	};

	class RemovalFaceVertexShader : public VertexShaderBase
	{
	public:
		RemovalFaceVertexShader();

		virtual void Calculate() override;
	};

	class RemovalFacePixelShader : public PixelShaderBase
	{
	public:
		RemovalFacePixelShader();

		virtual void Calculate() override;
	};
}
