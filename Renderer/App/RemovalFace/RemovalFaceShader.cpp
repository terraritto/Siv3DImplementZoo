#include "RemovalFaceShader.h"

namespace TerakoyaRenderer::RemovalFace
{
	RemovalFaceVertexShader::RemovalFaceVertexShader()
	{
		m_input = std::make_shared<RemovalFaceVertexInput>();
		m_output = std::make_shared<RemovalFaceVertexOutput>();
	}

	void RemovalFaceVertexShader::Calculate()
	{
		// 入力と出力
		auto input = std::static_pointer_cast<RemovalFaceVertexInput>(m_input);
		auto output = std::static_pointer_cast<RemovalFaceVertexOutput>(m_output);

		// projection空間まで持っていく
		Vec4 vert{ input->position, 1.0f };
		auto tempV = DirectX::XMVector4Transform(DirectX::XMVectorSet(vert.x, vert.y, vert.z, vert.w), input->worldMatrix);
		output->worldPos = Vec4{ tempV.m128_f32[0], tempV.m128_f32[1], tempV.m128_f32[2], tempV.m128_f32[3] };
		tempV = DirectX::XMVector4Transform(tempV, input->viewMatrix.transposed());
		output->viewPos = Vec4{ tempV.m128_f32[0], tempV.m128_f32[1], tempV.m128_f32[2], tempV.m128_f32[3] };
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
		output->screenPos = Vec4{ tempV.m128_f32[0], tempV.m128_f32[1],tempV.m128_f32[2],tempV.m128_f32[3] };
	}

	RemovalFacePixelShader::RemovalFacePixelShader()
	{
		// 入力のみでOK
		m_input = std::make_shared<RemovalFacePixelInput>();
	}

	void RemovalFacePixelShader::Calculate()
	{
		if (m_image == nullptr) { return; }

		auto input = std::static_pointer_cast<RemovalFacePixelInput>(m_input);

		double NoL = Abs(Dot(-input->directionalLight, input->normal));
		(*m_image)[input->screenPos.y][input->screenPos.x] = ColorF({ NoL * input->lightColor,1.0 });
	}
}
