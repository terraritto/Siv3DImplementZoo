#include "PhongShader.h"

namespace TerakoyaRenderer::Shading
{
	PhongVertexShader::PhongVertexShader()
	{
		m_input = std::make_shared<PhongVertexInput>();
		m_output = std::make_shared<PhongVertexOutput>();
	}

	void PhongVertexShader::Calculate()
	{
		// 入力と出力
		auto input = std::static_pointer_cast<PhongVertexInput>(m_input);
		auto output = std::static_pointer_cast<PhongVertexOutput>(m_output);

		// Projection空間へ
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

		// ライトの計算をWorld空間でしておく
		Vec4 normal{ input->normal, 1.0f };
		auto tempN = DirectX::XMVector4Transform(DirectX::XMVectorSet(normal.x, normal.y, normal.z, normal.w), input->worldMatrix);
		output->worldNormal = Vec3{ tempN.m128_f32[0], tempN.m128_f32[1], tempN.m128_f32[2] };
	}

	PhongPixelShader::PhongPixelShader()
	{
		// 入力のみでOK
		m_input = std::make_shared<PhongPixelInput>();
	}

	void PhongPixelShader::Calculate()
	{
		if (m_image == nullptr) { return; }

		auto input = std::static_pointer_cast<PhongPixelInput>(m_input);

		double NoL = Abs(Dot(-input->directionalLight, input->normal));
		(*m_image)[input->screenPos.y][input->screenPos.x] = ColorF({ NoL * input->lightColor,1.0 });
	}
}
