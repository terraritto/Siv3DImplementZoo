#pragma once

#pragma once
#include "RenderAppBase.h"
#include "../Common/Shader/TestShader.h"

namespace TerakoyaRenderer
{
	class SurtherlandHodgemanApp : public RenderAppBase
	{
	public:
		using RenderAppBase::RenderAppBase;

	public:
		virtual void Prepare() override;
		virtual void Render() override;

	protected:
		void FillDrawProcess(const Array<Vec4>& tempVertices, const Array<unsigned int>& tempIndices);
		double CalculateEdge(const Vec2& p, const Vec2& origin, const float dx, const float dy) const;
		bool IsInside(const Vec2& p, const Vec2& origin, const double dx, const double dy) const;

	private:
		Array<Vec2> SurtherlandHodgeman(const Array<Vec2>& clips, const Array<Vec2>& subjects);

	private:
		std::unique_ptr<TestVertexShader> m_vertexShader;
	};
}
