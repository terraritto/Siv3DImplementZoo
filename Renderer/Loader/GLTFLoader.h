#pragma once
#include "Loader.h"
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
//#define TINYGLTF_IMPLEMENTATION
#include "./tiny_gltf.h"


namespace TerakoyaRenderer
{
	class GLTFLoader : public Loader
	{
	public:
		virtual bool Load(String path, std::weak_ptr<RenderAppBase> app) override;

	private:
		void FetchMeshes(tinygltf::Model& model);
		void FetchImages(tinygltf::Model& model);
		void FetchMaterials(tinygltf::Model& model);

	private:
		std::weak_ptr<Model> m_model;
		String m_path;
	};
}
