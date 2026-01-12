#include "GLTFLoader.h"
#include "../Common/Mesh.h"
#include "../Common/Model.h"
#include "../Common/Primitive.h"
#include "../Common/Scene.h"
#include "../Common/SceneNode.h"
#include "../Common/Vertex.h"

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
//#define TINYGLTF_IMPLEMENTATION
#include "./tiny_gltf.h"

namespace TerakoyaRenderer
{
	bool GLTFLoader::Load(String path, std::weak_ptr<RenderAppBase> app)
	{
		tinygltf::Model model;
		tinygltf::TinyGLTF loader;
		std::string error, warning;

		bool isLoad = loader.LoadASCIIFromFile(&model, &error, &warning, path.toUTF8());

		if (isLoad == false)
		{
			Logger << path << U"is can't load.";
			return false;
		}

		std::shared_ptr<RenderAppBase> renderManager = app.lock();
		std::shared_ptr<Model> renderModel = renderManager->CreateModel().lock();

		// Meshの生成
		for (auto& mesh : model.meshes)
		{
			std::shared_ptr<Mesh> renderMesh = renderModel->CreateMesh().lock();

			for (auto& primitive : mesh.primitives)
			{
				std::shared_ptr<Primitive> renderPrimitive = renderMesh->CreatePrimitive().lock();

				// 各種Indexを取得
				auto& positionIndex = primitive.attributes["POSITION"];
				auto& normalIndex = primitive.attributes["NORMAL"];
				auto& indicesIndex = primitive.indices;

				// データ取得
				auto& positionAccessor = model.accessors[positionIndex];
				auto& positionBufferView = model.bufferViews[positionAccessor.bufferView];
				auto& positionBuffer = model.buffers[positionBufferView.buffer];
				const float* positions = reinterpret_cast<const float*>(&positionBuffer.data[positionBufferView.byteOffset + positionAccessor.byteOffset]);

				auto& normalAccessor = model.accessors[normalIndex];
				auto& normalBufferView = model.bufferViews[normalAccessor.bufferView];
				auto& normalBuffer = model.buffers[normalBufferView.buffer];
				const float* normals = reinterpret_cast<const float*>(&normalBuffer.data[normalBufferView.byteOffset + normalAccessor.byteOffset]);

				for (int i = 0; i < positionAccessor.count; ++i)
				{
					int index = i * 3 + 0;
					auto vertex = std::make_shared<Vertex>();
					vertex->m_position = Vec3{ positions[index], positions[index + 1], positions[index + 2] };
					vertex->m_normal = normalAccessor.normalized ?
						Vec3{ normals[index], normals[index + 1], normals[index + 2] } :
						Vec3{ normals[index], normals[index + 1], normals[index + 2] }.normalized();
					renderPrimitive->AddVertex(vertex);
				}

				// indexの取得
				auto& indicesAccessor = model.accessors[indicesIndex];
				auto& indicesBufferView = model.bufferViews[indicesAccessor.bufferView];
				auto& indicesBuffer = model.buffers[indicesBufferView.buffer];

				switch (indicesAccessor.componentType)
				{
				case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
				{
					const unsigned short* indices = reinterpret_cast<const unsigned short*>(&indicesBuffer.data[indicesBufferView.byteOffset + indicesAccessor.byteOffset]);

					for (unsigned long long i = 0; i < indicesAccessor.count; ++i)
					{
						renderPrimitive->AddIndex(indices[i]);
					}
				}
				break;
				case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT:
				{
					const unsigned int* indices = reinterpret_cast<const unsigned int*>(&indicesBuffer.data[indicesBufferView.byteOffset + indicesAccessor.byteOffset]);

					for (unsigned long long i = 0; i < indicesAccessor.count; ++i)
					{
						renderPrimitive->AddIndex(indices[i]);
					}
				}
				break;
				}
			}		
		}

		for (auto& gltfScene : model.scenes)
		{
			// Sceneを用意
			std::shared_ptr<Scene> scene = renderModel->CreateScene().lock();
			std::shared_ptr<SceneNode> root = scene->AddRoot().lock();

			std::queue<std::pair<int, std::shared_ptr<SceneNode>>> nodeQueue;
			for (auto& nodeIndex : gltfScene.nodes)
			{
				nodeQueue.push({ nodeIndex, root });
			}

			while (nodeQueue.empty() == false)
			{
				// 子の生成
				auto nodeData = nodeQueue.front(); nodeQueue.pop();
				std::shared_ptr<SceneNode> node = nodeData.second->AddChildren().lock();

				// ノードを取得
				auto& currentNode = model.nodes[nodeData.first];

				// データをセット
				node->SetMatrix(nodeData.second->GetMatrix()); // 親の状態を更新しておく
				if (currentNode.rotation.size() != 0)
				{
					node->SetRotation(Quaternion{ currentNode.rotation[0], currentNode.rotation[1], currentNode.rotation[2], currentNode.rotation[3] });
				}
				if (currentNode.translation.size() != 0)
				{
					node->SetTranslation({ currentNode.translation[0], currentNode.translation[1], currentNode.translation[2] });
				}
				node->SetParent(nodeData.second);
				node->SetMeshIndex(currentNode.mesh);

				// 子を設定する
				for (auto& child : currentNode.children)
				{
					nodeQueue.push({ child, node });
				}
			}
		}

		return true;
	}
}
