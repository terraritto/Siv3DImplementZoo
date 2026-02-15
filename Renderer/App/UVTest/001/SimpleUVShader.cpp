#include "SimpleUVShader.h"

namespace TerakoyaRenderer::UV
{
	SimpleUVVertexShader::SimpleUVVertexShader()
	{
		m_input = std::make_shared<SimpleUVVertexInput>();
		m_output = std::make_shared<SimpleUVVertexOutput>();
	}

	void SimpleUVVertexShader::Calculate()
	{
		// 入力と出力
		auto input = std::static_pointer_cast<SimpleUVVertexInput>(m_input);
		auto output = std::static_pointer_cast<SimpleUVVertexOutput>(m_output);

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
		output->uv = input->uv;

		Vec4 normal{ input->normal, 1.0f };
		auto tempN = DirectX::XMVector4Transform(DirectX::XMVectorSet(normal.x, normal.y, normal.z, normal.w), input->worldMatrix);
		output->worldNormal = Vec3{ tempN.m128_f32[0], tempN.m128_f32[1], tempN.m128_f32[2] };
	}

	SimpleUVPixelShader::SimpleUVPixelShader()
	{
		// 入力のみでOK
		m_input = std::make_shared<SimpleUVPixelInput>();
	}

	void SimpleUVPixelShader::Calculate()
	{
		if (m_image == nullptr) { return; }

		auto input = std::static_pointer_cast<SimpleUVPixelInput>(m_input);

		auto baseColorTexture = input->baseColorTexture.lock();

		Vec3 baseColor = Vec3::One();
		if (baseColorTexture)
		{
			auto h = baseColorTexture->height() - 1;
			auto w = baseColorTexture->width() - 1;
			Color baseColorData = (*baseColorTexture)
				[static_cast<int>(h * input->uv.y)][static_cast<int>(w * input->uv.x)];
			baseColor = Vec3{ baseColorData.r, baseColorData.g, baseColorData.b } / 255.0f;
		}

		double NoL = Abs(Dot(-input->directionalLight, input->normal));
		(*m_image)[input->screenPos.y][input->screenPos.x] = ColorF({ baseColor ,1.0 });
	}
}
