#pragma once
#include "GSDefine.h"
#include "../RenderAppBase.h"
#include "../../Common/Shader/TestShader.h"

namespace TerakoyaRenderer::GS
{
	class GSVertexApp : public RenderAppBase
	{
	public:
		using RenderAppBase::RenderAppBase;

	public:
		virtual void Prepare() override;
		virtual void Render() override;

	protected:
		void FillDrawProcess(const Array<Vec4>& tempVertices, const Array<unsigned int>& tempIndices);
		double CalculateEdge(const Vec2& p, const Vec2& origin, const float dx, const float dy) const;
		bool IsInside(const Vec2& p, const Vec2& origin, const float dx, const float dy) const;

		void LoadPly(const String& path);

	private:
		GSData m_gsData;
		GSCalculateData m_gsCalculateData;
	};
}
