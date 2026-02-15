#include "RenderAppBase.h"
#include "../Common/Camera.h"
#include "../Common/Model.h"
#include "../Common/DrawMethod/BresenhamDrawMethod.h"
#include "../Common/DrawMethod/PointDrawMethod.h"

namespace TerakoyaRenderer
{
	RenderAppBase::RenderAppBase()
		: m_drawMethod(EDrawMethod::Line)
	{
		m_camera = std::make_shared<Camera>();
		m_renderImage = Image{ s3d::Scene::Size(), Palette::Black };
		m_renderTexture = DynamicTexture{ m_renderImage };

		// DrawMethodの生成
		m_drawMethodMap[EDrawMethod::Line] = std::make_shared<BresenhamDrawMethod>();
		m_drawMethodMap[EDrawMethod::Point] = std::make_shared<PointDrawMethod>();
	}

	RenderAppBase::~RenderAppBase()
	{
	}

	std::weak_ptr<Model> RenderAppBase::CreateModel()
	{
		std::shared_ptr<Model> model = std::make_shared<Model>();
		m_modelList.push_back(model);
		return model;
	}

	void RenderAppBase::SetDrawMethod(const EDrawMethod drawMethod)
	{
		m_drawMethod = drawMethod;
	}

	void RenderAppBase::Draw()
	{
		m_renderTexture.draw();
	}

	void RenderAppBase::Destroy()
	{
		m_modelList.clear();
	}

	bool RenderAppBase::IsCorrectRegionVertex(const int width, const int height, const Vec2& v)
	{
		if (v.x >= width || v.x < 0) { return false; }
		if (v.y >= height || v.y < 0) { return false; }
		if (IsNaN(v.x) || IsNaN(v.y)) { Print << U"Nan"; return false; }
		return true;
	}

	void RenderAppBase::SpecialDrawMethodProcess(const Array<Vec4>& tempVertices, const Array<unsigned int>& tempIndices)
	{
		std::shared_ptr<IDrawMethod> drawMethod = m_drawMethodMap[m_drawMethod];

		// Pixel Shader
		for (int i = 0; i < tempIndices.size(); i += 3)
		{
			const Vec4& v1 = tempVertices[tempIndices[i]];
			const Vec4& v2 = tempVertices[tempIndices[i + 1]];
			const Vec4& v3 = tempVertices[tempIndices[i + 2]];

			if (
				IsCorrectRegionVertex(m_renderImage.width() - 1, m_renderImage.height() - 1, v1.xy()) == false ||
				IsCorrectRegionVertex(m_renderImage.width() - 1, m_renderImage.height() - 1, v2.xy()) == false ||
				IsCorrectRegionVertex(m_renderImage.width() - 1, m_renderImage.height() - 1, v3.xy()) == false
				)
			{
				continue;
			}

			drawMethod->Draw(v1.xy(), v2.xy(), v3.xy(), m_renderImage);
		}
	}
}
