#include "GltfTestApp.h"
#include "../Common/Camera.h"
#include "../Common/Mesh.h"
#include "../Common/Model.h"
#include "../Common/Primitive.h"
#include "../Common/Scene.h"
#include "../Common/SceneNode.h"
#include "../Common/Vertex.h"
#include "../Common/DrawMethod/BresenhamDrawMethod.h"
#include "../Common/DrawMethod/PointDrawMethod.h"
#include "../Loader/GLTFLoader.h"

namespace TerakoyaRenderer
{
	void GltfTestApp::Prepare()
	{
		TerakoyaRenderer::GLTFLoader loader;
		loader.Load(U"./example/obj/ABeautifulGame/glTF/ABeautifulGame.gltf", shared_from_this());

		SetDrawMethod(TerakoyaRenderer::EDrawMethod::Point);

		m_camera->SetOrigin(Vec3{ -0.5f, 0.3f, 0.0f });
		m_camera->SetLookAt(Vec3::Zero());
	}

	void GltfTestApp::Render()
	{
		// @todo: まずはべた書きで動くのを目指す
		// @todo: 徐々に処理を分割していく予定
		for (auto& model : m_modelList)
		{
			auto scene = model->GetScene().lock();
			if (scene == nullptr) { return; }
			auto root = scene->GetRoot().lock();
			if (root == nullptr) { return; }

			std::queue<std::shared_ptr<SceneNode>> nodes;
			for (auto& child : root->GetChildren())
			{
				nodes.push(child);
			}

			while (nodes.empty() == false)
			{
				auto node = nodes.front(); nodes.pop();
				for (auto& child : node->GetChildren())
				{
					nodes.push(child);
				}

				Mat4x4 worldMatrix = node->GetMatrix();
				auto mesh = model->GetMesh(node->GetMeshIndex()).lock();
				auto primitives = mesh->GetPrimitives();

				Array<Vec4> tempVertices;
				Array<Vec4> tempWorldVertices;
				Mat4x4 viewMatrix = m_camera->GetViewMatrix();
				Mat4x4 projMatrix = m_camera->GetProjMatrix();

				double ff = 1.0, nn = 0.0;
				Mat4x4 viewportMat = Mat4x4::Set
				(
					s3d::Scene::Size().x / 2.0, 0, 0, s3d::Scene::Size().x / 2.0 + 0.0,
					0, -s3d::Scene::Size().y / 2.0, 0.0, s3d::Scene::Size().y / 2.0 + 0.0,
					0, 0, (ff - nn) / 2.0, (ff + nn) / 2.0,
					0, 0, 0, 1
				);

				for (auto& primitive : primitives)
				{
					const auto& vertices = primitive->GetVertices();
					const auto& indices = primitive->GetIndices();
					tempVertices.resize(vertices.size());
					tempWorldVertices.resize(vertices.size());

					// Vertex Shader相当の処理
					for (int i = 0; auto & vertex : vertices)
					{
						Vec4 vert{ vertex->m_position, 1.0f };

						auto tempV = DirectX::XMVector4Transform(DirectX::XMVectorSet(vert.x, vert.y, vert.z, vert.w), worldMatrix);
						tempWorldVertices[i] = Vec4{ tempV.m128_f32[0], tempV.m128_f32[1],tempV.m128_f32[2],tempV.m128_f32[3] };
						tempV = DirectX::XMVector4Transform(tempV, viewMatrix.transposed());
						tempV = DirectX::XMVector4Transform(tempV, projMatrix.transposed());
						// 正規化デバイス座標へ
						tempV.m128_f32[0] = tempV.m128_f32[0] / tempV.m128_f32[3];
						tempV.m128_f32[1] = tempV.m128_f32[1] / tempV.m128_f32[3];
						tempV.m128_f32[2] = tempV.m128_f32[2] / tempV.m128_f32[3];
						tempV.m128_f32[3] = 1.0f;

						tempV = DirectX::XMVector4Transform(tempV, viewportMat.transposed());

						tempVertices[i] = Vec4{ tempV.m128_f32[0], tempV.m128_f32[1],tempV.m128_f32[2],tempV.m128_f32[3] };
						i++;
					}

					// 簡易描画
					SpecialDrawMethodProcess(tempVertices, indices);

					tempVertices.clear();
					tempWorldVertices.clear();
				}
			}
		}

		m_renderTexture.fill(m_renderImage);
	}
}
