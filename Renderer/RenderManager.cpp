#include "RenderManager.h"
#include "Common/Camera.h"
#include "Common/Mesh.h"
#include "Common/Model.h"
#include "Common/Primitive.h"
#include "Common/Scene.h"
#include "Common/SceneNode.h"
#include "Common/Vertex.h"
#include "Common/DrawMethod/BresenhamDrawMethod.h"
#include "Common/DrawMethod/PointDrawMethod.h"

namespace TerakoyaRenderer
{
	std::shared_ptr<RenderManager> RenderManager::m_instance = nullptr;

	RenderManager::RenderManager()
		: m_drawMethod(EDrawMethod::Line)
	{
		m_camera = std::make_shared<Camera>();
		m_renderImage = Image{ s3d::Scene::Size(), Palette::Black };
		m_renderTexture = DynamicTexture{ m_renderImage };

		// DrawMethodの生成
		m_drawMethodMap[EDrawMethod::Line] = std::make_shared<BresenhamDrawMethod>();
		m_drawMethodMap[EDrawMethod::Point] = std::make_shared<PointDrawMethod>();
	}

	RenderManager::~RenderManager()
	{
	}

	std::weak_ptr<Model> TerakoyaRenderer::RenderManager::CreateModel()
	{
		std::shared_ptr<Model> model = std::make_shared<Model>();
		m_modelList.push_back(model);
		return model;
	}

	void RenderManager::SetDrawMethod(const EDrawMethod drawMethod)
	{
		m_drawMethod = drawMethod;
	}

	void RenderManager::Render()
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
					for (int i = 0; auto& vertex : vertices)
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

					// @todo: Pixel Shaderを別途できるようにする
					auto IsCorrectRegionVertex = [](const int width, const int height, const Vec4& v)
						{
							if (v.x >= width || v.x < 0) { return false; }
							if (v.y >= height || v.y < 0) { return false; }
							if (IsNaN(v.x) || IsNaN(v.y)) { Print << U"Nan"; return false; }
							return true;
						};

					std::shared_ptr<IDrawMethod> drawMethod = m_drawMethodMap[m_drawMethod];

					for (int i = 0; i < indices.size(); i += 3)
					{
						const Vec4& v1 = tempVertices[indices[i]];
						const Vec4& v2 = tempVertices[indices[i+1]];
						const Vec4& v3 = tempVertices[indices[i+2]];

						// 見えない面は消しておく
						Vec3 meshNormal =
							Cross((tempWorldVertices[indices[i+1]] - tempWorldVertices[indices[i]]).normalize().xyz(),
								(tempWorldVertices[indices[i + 2]] - tempWorldVertices[indices[i]]).normalize().xyz());
						Vec3 cameraRay = m_camera->GetCameraRay();
						if (Dot(meshNormal, cameraRay) > 0.0)
						{
							continue;
						}

						if (
							IsCorrectRegionVertex(m_renderImage.width() - 1, m_renderImage.height() - 1, v1) == false ||
							IsCorrectRegionVertex(m_renderImage.width() - 1, m_renderImage.height() - 1, v2) == false ||
							IsCorrectRegionVertex(m_renderImage.width() - 1, m_renderImage.height() - 1, v3) == false
							)
						{
							continue;
						}

						drawMethod->Draw(v1.xy(), v2.xy(), v3.xy(), m_renderImage);
					}

					tempVertices.clear();
					tempWorldVertices.clear();
				}
			}
		}

		m_renderTexture.fill(m_renderImage);
	}

	void RenderManager::Draw()
	{
		m_renderTexture.draw();
	}
}
