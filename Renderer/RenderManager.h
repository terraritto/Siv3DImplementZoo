#pragma once

#include <Siv3D.hpp>

namespace TerakoyaRenderer
{
	class Model;
	class Camera;
	class IDrawMethod;
	enum class EDrawMethod;

	class RenderManager
	{
	public:
		RenderManager();
		~RenderManager();

		// 提供関係
		static std::weak_ptr<RenderManager> GetInstance()
		{
			return m_instance;
		}

		static void Create()
		{
			if (!m_instance)
			{
				m_instance = std::make_shared<RenderManager>();
			}
		}

		static void Destroy()
		{
			m_instance.reset();
		}

	public:
		std::weak_ptr<Model> CreateModel();

		// Getter/Setter
		std::weak_ptr<Camera> GetCamera() { return m_camera; }
		void SetDrawMethod(const EDrawMethod drawMethod);

		void Render();
		void Draw();

	private:
		static std::shared_ptr<RenderManager> m_instance;

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
