#pragma once
#include <Siv3D.hpp>

namespace TerakoyaRenderer
{

	struct ReceiveParameterBase
	{
	};

	class ShaderBase
	{
	public:
		ShaderBase();

		// 実際のShader内の計算を記述する
		virtual void Calculate() = 0;

		// 入力となるShader Parameterの取得
		std::weak_ptr<ReceiveParameterBase> GetShaderInput() const { return m_input; }

		// 出力となるShader Parameterの取得
		std::weak_ptr<ReceiveParameterBase> GetShaderOutput() const { return m_output; }

	protected:
		// Shaderの入力/出力
		std::shared_ptr<ReceiveParameterBase> m_input;
		std::shared_ptr<ReceiveParameterBase> m_output;
	};
}
