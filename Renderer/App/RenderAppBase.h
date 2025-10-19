#pragma once

#include <Siv3D.hpp>

namespace TerakoyaRenderer
{
	class Model;
	class Camera;
	class IDrawMethod;
	enum class EDrawMethod;

	class RenderAppBase : public std::enable_shared_from_this<RenderAppBase>
	{
	public:
		RenderAppBase();
		~RenderAppBase();

	public:
		std::weak_ptr<Model> CreateModel();

		// Getter/Setter
		std::weak_ptr<Camera> GetCamera() { return m_camera; }
		void SetDrawMethod(const EDrawMethod drawMethod);

		virtual void Prepare() = 0;
		virtual void Render() = 0;
		void Draw();

		virtual void Destroy();

	protected:
		// 頂点が範囲内かの判定
		bool IsCorrectRegionVertex(const int width, const int height, const Vec2& v);

		// Point/Lineの描画処理
		void SpecialDrawMethodProcess(const Array<Vec4>& tempVertices, const Array<unsigned int>& tempIndices);

	protected:
		// 保有するモデル情報
		Array<std::shared_ptr<Model>> m_modelList;

		// Camera
		std::shared_ptr<Camera> m_camera;

		// DrawMethod
		std::map<EDrawMethod, std::shared_ptr<IDrawMethod>> m_drawMethodMap;
		EDrawMethod m_drawMethod;

		// RenderTarget
		Image m_renderImage;
		DynamicTexture m_renderTexture;
	};
}
