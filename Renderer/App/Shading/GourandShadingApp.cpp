#include "GourandShadingApp.h"
#include "../../Common/Camera.h"
#include "../../Common/Mesh.h"
#include "../../Common/Model.h"
#include "../../Common/Primitive.h"
#include "../../Common/Scene.h"
#include "../../Common/SceneNode.h"
#include "../../Common/Vertex.h"
#include "../../Common/DrawMethod/BresenhamDrawMethod.h"
#include "../../Common/DrawMethod/PointDrawMethod.h"
#include "../../Loader/GLTFLoader.h"

namespace TerakoyaRenderer::Shading
{
	void GourandShadingApp::Prepare()
	{
		TerakoyaRenderer::GLTFLoader loader;
		loader.Load(U"./example/obj/ABeautifulGame/glTF/ABeautifulGame.gltf", shared_from_this());

		SetDrawMethod(TerakoyaRenderer::EDrawMethod::Fill);

		m_camera->SetOrigin(Vec3{ 0.7f, 0.2f, 0.0f });
		m_camera->SetLookAt(Vec3::Zero());
		m_camera->SetUp(Vec3::Up());
		m_camera->SetNear(0.00001);
		m_camera->SetFar(1000);

		m_lightDir = Vec3{ 0.8, -0.5, 0.3 }.normalized();
		m_lightColor = Vec3::One() * 0.8;

		m_depthBuffer = std::make_unique<DepthBuffer>(::Scene::Size().x, ::Scene::Size().y);

		m_vertexShader = std::make_unique<GourandVertexShader>();
		m_pixelShader = std::make_unique<GourandPixelShader>();
		m_pixelShader->SetImage(&m_renderImage);

		if (auto shaderInput = std::static_pointer_cast<GourandVertexInput>(m_vertexShader->GetShaderInput().lock()))
		{
			shaderInput->viewMatrix = m_camera->GetViewMatrix();
			shaderInput->projectionMatrix = m_camera->GetProjMatrix();

			constexpr float ff = 1.0f, nn = 0.0f;
			shaderInput->viewportMatrix = Mat4x4::Set
			(
				s3d::Scene::Size().x / 2.0, 0, 0, s3d::Scene::Size().x / 2.0 + 0.0,
				0, -s3d::Scene::Size().y / 2.0, 0.0, s3d::Scene::Size().y / 2.0 + 0.0,
				0, 0, (ff - nn) / 2.0, (ff + nn) / 2.0,
				0, 0, 0, 1
			);
		}
	}

	void GourandShadingApp::Render()
	{
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
				Array<std::shared_ptr<GourandVertexOutput>> tempVertexOutputs;

				for (auto& primitive : primitives)
				{
					const auto& vertices = primitive->GetVertices();
					const auto& indices = primitive->GetIndices();
					tempVertices.resize(vertices.size());
					tempVertexOutputs.resize(vertices.size());

					// Vertex Shader相当の処理
					for (int i = 0; auto& vertex : vertices)
					{
						auto shaderInput = std::static_pointer_cast<GourandVertexInput>(m_vertexShader->GetShaderInput().lock());
						if (shaderInput == nullptr)
						{
							continue;
						}

						shaderInput->worldMatrix = worldMatrix;
						shaderInput->position = vertex->m_position;

						// Gourand Shadingは頂点でライティングを計算
						// そのため、ライトを計算しておく
						shaderInput->normal = vertex->m_normal;
						shaderInput->lightColor = m_lightColor;
						shaderInput->directionalLight = m_lightDir;

						m_vertexShader->Calculate();

						auto shaderOutput = std::static_pointer_cast<GourandVertexOutput>(m_vertexShader->GetShaderOutput().lock());
						if (shaderOutput == nullptr)
						{
							continue;
						}

						tempVertices[i] = shaderOutput->screenPos;
						tempVertexOutputs[i] = std::make_shared<GourandVertexOutput>(*shaderOutput);
						i++;
					}

					switch (m_drawMethod)
					{
					case EDrawMethod::Point:
					case EDrawMethod::Line:
						SpecialDrawMethodProcess(tempVertices, indices);
						break;

					case EDrawMethod::Fill:
						FillDrawProcess(tempVertexOutputs, indices);
						break;

					default:
						break;
					}

					tempVertices.clear();
					tempVertexOutputs.clear();
				}
			}
		}

		m_renderTexture.fill(m_renderImage);
	}

	void GourandShadingApp::FillDrawProcess(const Array<std::shared_ptr<GourandVertexOutput>>& tempVertices, const Array<unsigned int>& tempIndices)
	{
		auto shaderInput = std::static_pointer_cast<GourandPixelInput>(m_pixelShader->GetShaderInput().lock());
		for (unsigned int i = 0; i < tempIndices.size(); i += 3)
		{
			const auto& vert1 = tempVertices[tempIndices[i]];
			const auto& vert2 = tempVertices[tempIndices[i + 1]];
			const auto& vert3 = tempVertices[tempIndices[i + 2]];

			const auto& v1 = vert1->screenPos;
			const auto& v2 = vert2->screenPos;
			const auto& v3 = vert3->screenPos;

			// BBoxを計算
			Vec2 BBoxX = { Max(Min({v1.x, v2.x, v3.x}), 0.0), Min(Max({v1.x, v2.x, v3.x}), static_cast<double>(m_renderImage.width() - 1)) };
			Vec2 BBoxY = { Max(Min({v1.y, v2.y, v3.y}), 0.0), Min(Max({v1.y, v2.y, v3.y}), static_cast<double>(m_renderImage.height() - 1)) };

			// 辺の生成
			double dx12 = v2.x - v1.x, dy12 = v2.y - v1.y;
			double dx23 = v3.x - v2.x, dy23 = v3.y - v2.y;
			double dx31 = v1.x - v3.x, dy31 = v1.y - v3.y;

			for (int x = BBoxX.x; x <= BBoxX.y; x++)
			{
				for (int y = BBoxY.x; y <= BBoxY.y; y++)
				{
					Vec2 p{ x,y };
					if (IsInside(p, v1.xy(), dx12, dy12) == false) { continue; }
					if (IsInside(p, v2.xy(), dx23, dy23) == false) { continue; }
					if (IsInside(p, v3.xy(), dx31, dy31) == false) { continue; }

					// 重心の計算
					const double polyArea = Cross(v2.xy() - v1.xy(), v3.xy() - v1.xy());
					if (polyArea == 0.0) { continue; } // ゼロ除算対策
					const double u = Abs(Cross(v2.xy() - v1.xy(), p - v1.xy()) / polyArea); // v3用
					const double v = Abs(Cross(v3.xy() - v1.xy(), p - v1.xy()) / polyArea); // v2用
					const double w = Abs(Cross(v3.xy() - v2.xy(), p - v2.xy()) / polyArea); // v1用

					// 重心による補間
					const double depth = w * vert1->viewPos.z + v * vert2->viewPos.z + u * vert3->viewPos.z;

					// 深度バッファの適用
					if (depth < m_depthBuffer->m_data[y][x] || depth >= 0.0)
					{
						continue;
					}
					m_depthBuffer->m_data[y][x] = depth;

					// ライト結果を補間
					const Vec3 light = w * vert1->light + v * vert2->light + u * vert3->light;
					shaderInput->light = light;

					// パラメータを用意
					shaderInput->screenPos = { x,y };
					m_pixelShader->Calculate();
				}
			}
		}
	}

	double GourandShadingApp::CalculateEdge(const Vec2& p, const Vec2& origin, const float dx, const float dy) const
	{
		return (p.x - origin.x) * dy - (p.y - origin.y) * dx;
	}

	bool GourandShadingApp::IsInside(const Vec2& p, const Vec2& origin, const double dx, const double dy) const
	{
		double edge = CalculateEdge(p, origin, dx, dy);
		if (Abs(edge) < 0.00001) { return true; }

		return 0.0 < edge;
	}
}
