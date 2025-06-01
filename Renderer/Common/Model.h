#pragma once
#include <Siv3D.hpp>

namespace TerakoyaRenderer
{
	class Scene;
	class Mesh;

	class Model
	{
	public:
		// 所有するSceneの生成
		std::weak_ptr<Scene> CreateScene();

		// 所有するMeshの生成
		std::weak_ptr<Mesh> CreateMesh();

		// Sceneの取得
		size_t GetSceneNum();
		std::weak_ptr<Scene> GetScene(size_t index = 0);
		std::weak_ptr<Mesh> GetMesh(size_t index);

	protected:
		Array<std::shared_ptr<Scene>> m_scenes;
		Array<std::shared_ptr<Mesh>> m_meshes;
	};
}
