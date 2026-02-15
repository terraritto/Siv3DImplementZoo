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

	std::weak_ptr<DefaultMaterial> Model::CreateMaterial()
	{
		std::shared_ptr<DefaultMaterial> material = std::make_shared<DefaultMaterial>();
		m_materials.push_back(material);
		return material;
	}

	std::weak_ptr<ImageHolder> Model::CreateImageHolder()
	{
		m_imageHolder = std::make_shared<ImageHolder>();
		return m_imageHolder;
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

	std::weak_ptr<DefaultMaterial> Model::GetMaterial(size_t index)
	{
		return m_materials[index];
	}

	std::weak_ptr<Image> Model::GetImage(size_t index)
	{
		return m_imageHolder->GetImage(index);
	}
}
