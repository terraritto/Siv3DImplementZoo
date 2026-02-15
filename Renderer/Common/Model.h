#pragma once
#include <Siv3D.hpp>
#include "Scene.h"
#include "Mesh.h"
#include "ImageHolder.h"
#include "Material/DefaultMaterial.h"

namespace TerakoyaRenderer
{
	class Model
	{
	public:
		//Model();

		// Sceneの取得
		size_t GetSceneNum();
		std::weak_ptr<Scene> GetScene(size_t index = 0);
		std::weak_ptr<Mesh> GetMesh(size_t index);
		std::weak_ptr<DefaultMaterial> GetMaterial(size_t index);
		std::weak_ptr<Image> GetImage(size_t index);

	protected:
		// 所有するSceneの生成
		std::weak_ptr<Scene> CreateScene();

		// 所有するMeshの生成
		std::weak_ptr<Mesh> CreateMesh();

		// 所有するマテリアルの作成
		std::weak_ptr<DefaultMaterial> CreateMaterial();

		// 所有する画像の作成
		std::weak_ptr<ImageHolder> CreateImageHolder();

	protected:
		Array<std::shared_ptr<Scene>> m_scenes;
		Array<std::shared_ptr<Mesh>> m_meshes;
		Array<std::shared_ptr<DefaultMaterial>> m_materials;
		std::shared_ptr<ImageHolder> m_imageHolder;

	private:
		friend class GLTFLoader;
	};
}
