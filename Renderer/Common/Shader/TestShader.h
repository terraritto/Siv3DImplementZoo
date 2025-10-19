#pragma once
#include <Siv3D.hpp>
#include "VertexShaderBase.h"

namespace TerakoyaRenderer
{
	struct TestVertexInput : public ReceiveParameterBase
	{
		Vec3 position;
		Mat4x4 worldMatrix;
		Mat4x4 viewMatrix;
		Mat4x4 projectionMatrix;
		Mat4x4 viewportMatrix;
	};

	struct TestVertexOutput : public ReceiveParameterBase
	{
		Vec4 position;
	};

	class TestVertexShader : public VertexShaderBase
	{
	public:
		TestVertexShader();

		virtual void Calculate() override;
	};
}
