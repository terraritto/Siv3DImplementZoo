#include "Model.h"
#include "Scene.h"
#include "Mesh.h"

namespace TerakoyaRenderer
{
	std::weak_ptr<Scene> Model::CreateScene()
	{
		std::shared_ptr<Scene> scene = std::make_shared<Scene>();
		m_scenes.push_back(scene);
		return scene;
	}

	std::weak_ptr<Mesh> Model::CreateMesh()
	{
		std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>();
		m_meshes.push_back(mesh);
		return mesh;
	}

	size_t Model::GetSceneNum()
	{
		return m_scenes.size();
	}

	std::weak_ptr<Scene> Model::GetScene(size_t index)
	{
		return m_scenes[index];
	}

	std::weak_ptr<Mesh> Model::GetMesh(size_t index)
	{
		return m_meshes[index];
	}
}
