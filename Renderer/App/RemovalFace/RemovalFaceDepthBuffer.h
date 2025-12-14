#pragma once
#include "../RenderAppBase.h"
#include "../../Common/Buffer.h"
#include "RemovalFaceShader.h"

namespace TerakoyaRenderer::RemovalFace
{
	class RemovalFaceDepthBufferApp : public RenderAppBase
	{
	public:
		using RenderAppBase::RenderAppBase;

	public:
		virtual void Prepare() override;
		virtual void Render() override;

	protected:
		void FillDrawProcess(const Array<std::shared_ptr<RemovalFaceVertexOutput>>& tempVertices, const Array<unsigned int>& tempIndices);
		double CalculateEdge(const Vec2& p, const Vec2& origin, const float dx, const float dy) const;
		bool IsInside(const Vec2& p, const Vec2& origin, const double dx, const double dy) const;

	private:
		Vec3 m_lightDir;
		Vec3 m_lightColor;

		std::unique_ptr<DepthBuffer> m_depthBuffer;

	private:
		std::unique_ptr<RemovalFaceVertexShader> m_vertexShader;
		std::unique_ptr<RemovalFacePixelShader> m_pixelShader;
	};
}
