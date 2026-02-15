#pragma once
#include "../../RenderAppBase.h"
#include "../../../Common/Buffer.h"
#include "../../../Common/Material/DefaultMaterial.h"
#include "SimpleUVShader.h"

namespace TerakoyaRenderer::UV
{
	class SimpleUVApp : public RenderAppBase
	{
	public:
		using RenderAppBase::RenderAppBase;

	public:
		virtual void Prepare() override;
		virtual void Render() override;

	protected:
		void FillDrawProcess(const Array<std::shared_ptr<SimpleUVVertexOutput>>& tempVertices, const Array<unsigned int>& tempIndices, std::weak_ptr<DefaultMaterial> materialIndex);
		double CalculateEdge(const Vec2& p, const Vec2& origin, const float dx, const float dy) const;
		bool IsInside(const Vec2& p, const Vec2& origin, const double dx, const double dy) const;

	private:
		Vec3 m_lightDir;
		Vec3 m_lightColor;

		std::unique_ptr<DepthBuffer> m_depthBuffer;

	private:
		std::unique_ptr<SimpleUVVertexShader> m_vertexShader;
		std::unique_ptr<SimpleUVPixelShader> m_pixelShader;
	};
}
