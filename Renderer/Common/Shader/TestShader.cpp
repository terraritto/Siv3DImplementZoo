#include "TestShader.h"

namespace TerakoyaRenderer
{
	TestVertexShader::TestVertexShader()
	{
		m_input = std::make_shared<TestVertexInput>();
		m_output = std::make_shared<TestVertexOutput>();
	}

	void TestVertexShader::Calculate()
	{
		// 入力と出力
		auto input = std::static_pointer_cast<TestVertexInput>(m_input);
		auto output = std::static_pointer_cast<TestVertexOutput>(m_output);

		// projection空間まで持っていく
		Vec4 vert{ input->position, 1.0f };
		auto tempV = DirectX::XMVector4Transform(DirectX::XMVectorSet(vert.x, vert.y, vert.z, vert.w), input->worldMatrix);
		tempV = DirectX::XMVector4Transform(tempV, input->viewMatrix.transposed());
		tempV = DirectX::XMVector4Transform(tempV, input->projectionMatrix);

		// 正規化デバイス座標へ
		auto z = Abs(tempV.m128_f32[3]);
		tempV.m128_f32[0] = tempV.m128_f32[0] / z;
		tempV.m128_f32[1] = tempV.m128_f32[1] / z;
		tempV.m128_f32[2] = tempV.m128_f32[2] / z;
		tempV.m128_f32[3] = 1.0f;

		// viewport座標へ
		tempV = DirectX::XMVector4Transform(tempV, input->viewportMatrix.transposed());

		// 結果を格納
		output->position = Vec4{ tempV.m128_f32[0], tempV.m128_f32[1],tempV.m128_f32[2],tempV.m128_f32[3] };
	}
}
