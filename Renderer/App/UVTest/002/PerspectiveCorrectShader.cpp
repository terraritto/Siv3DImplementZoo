#include "PerspectiveCorrectShader.h"

namespace TerakoyaRenderer::UV
{
	PerspectiveCorrectVertexShader::PerspectiveCorrectVertexShader()
	{
		m_input = std::make_shared<PerspectiveCorrectVertexInput>();
		m_output = std::make_shared<PerspectiveCorrectVertexOutput>();
	}

	void PerspectiveCorrectVertexShader::Calculate()
	{
		// 入力と出力
		auto input = std::static_pointer_cast<PerspectiveCorrectVertexInput>(m_input);
		auto output = std::static_pointer_cast<PerspectiveCorrectVertexOutput>(m_output);

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
		output->screenPos = Vec4{ tempV.m128_f32[0], tempV.m128_f32[1],tempV.m128_f32[2], 1.0 / z };
		output->uv = input->uv / z;

		Vec4 normal{ input->normal, 1.0f };
		auto tempN = DirectX::XMVector4Transform(DirectX::XMVectorSet(normal.x, normal.y, normal.z, normal.w), input->worldMatrix);
		output->worldNormal = Vec3{ tempN.m128_f32[0], tempN.m128_f32[1], tempN.m128_f32[2] };
	}

	PerspectiveCorrectPixelShader::PerspectiveCorrectPixelShader()
	{
		// 入力のみでOK
		m_input = std::make_shared<PerspectiveCorrectPixelInput>();
	}

	void PerspectiveCorrectPixelShader::Calculate()
	{
		if (m_image == nullptr) { return; }

		auto input = std::static_pointer_cast<PerspectiveCorrectPixelInput>(m_input);

		auto baseColorTexture = input->baseColorTexture.lock();

		Vec3 baseColor = Vec3::One();
		if (baseColorTexture)
		{
			auto h = baseColorTexture->height() - 1;
			auto w = baseColorTexture->width() - 1;
			input->uv = Math::Clamp(input->uv, 0.0, 1.0);
			Color baseColorData = (*baseColorTexture)[static_cast<int>(h * input->uv.y)][static_cast<int>(w * input->uv.x)];
			baseColor = { baseColorData.r / 255.0, baseColorData.g / 255.0, baseColorData.b / 255.0 };
		}

		double NoL = Abs(Dot(-input->directionalLight, input->normal));
		(*m_image)[input->screenPos.y][input->screenPos.x] = ColorF({/* NoL * input->lightColor * */ baseColor ,1.0});
	}
}
