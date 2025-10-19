#include "SurtherlandHodgemanApp.h"

#include "SurtherlandHodgemanApp.h"
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
	void SurtherlandHodgemanApp::Prepare()
	{
		TerakoyaRenderer::GLTFLoader loader;
		loader.Load(U"./example/obj/ABeautifulGame/glTF/ABeautifulGame.gltf", shared_from_this());

		SetDrawMethod(TerakoyaRenderer::EDrawMethod::Fill);

		m_camera->SetOrigin(Vec3{ -0.5f, 0.3f, 0.0f });
		m_camera->SetLookAt(Vec3::Zero());

		m_vertexShader = std::make_unique<TestVertexShader>();

		if (auto shaderInput = std::static_pointer_cast<TestVertexInput>(m_vertexShader->GetShaderInput().lock()))
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

	void SurtherlandHodgemanApp::Render()
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
				Array<std::shared_ptr<TestVertexOutput>> tempVertexOutputs;

				for (auto& primitive : primitives)
				{
					const auto& vertices = primitive->GetVertices();
					const auto& indices = primitive->GetIndices();
					tempVertices.resize(vertices.size());
					tempVertexOutputs.resize(vertices.size());

					// Vertex Shader相当の処理
					for (int i = 0; auto & vertex : vertices)
					{
						auto shaderInput = std::static_pointer_cast<TestVertexInput>(m_vertexShader->GetShaderInput().lock());
						if (shaderInput == nullptr)
						{
							continue;
						}

						shaderInput->worldMatrix = worldMatrix;
						shaderInput->position = vertex->m_position;

						m_vertexShader->Calculate();

						auto shaderOutput = std::static_pointer_cast<TestVertexOutput>(m_vertexShader->GetShaderOutput().lock());
						if (shaderOutput == nullptr)
						{
							continue;
						}

						tempVertices[i] = shaderOutput->position;
						tempVertexOutputs[i] = std::make_shared<TestVertexOutput>(*shaderOutput);
						i++;
					}

					switch (m_drawMethod)
					{
					case EDrawMethod::Point:
					case EDrawMethod::Line:
						SpecialDrawMethodProcess(tempVertices, indices);
						break;

					case EDrawMethod::Fill:
						FillDrawProcess(tempVertices, indices);
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

	void SurtherlandHodgemanApp::FillDrawProcess(const Array<Vec4>& tempVertices, const Array<unsigned int>& tempIndices)
	{
		// Pixel Shader
		for (int i = 0; i < tempIndices.size(); i += 3)
		{
			const Vec4& v1 = tempVertices[tempIndices[i]];
			const Vec4& v2 = tempVertices[tempIndices[i + 1]];
			const Vec4& v3 = tempVertices[tempIndices[i + 2]];

			// Pattern1: 何もクリッピングされてない場合
			/*
			{
				// 頂点がそもそも範囲内？
				if (
					IsCorrectRegionVertex(m_renderImage.width() - 1, m_renderImage.height() - 1, v1.xy()) == false ||
					IsCorrectRegionVertex(m_renderImage.width() - 1, m_renderImage.height() - 1, v2.xy()) == false ||
					IsCorrectRegionVertex(m_renderImage.width() - 1, m_renderImage.height() - 1, v3.xy()) == false
					)
				{
					continue;
				}

				// BBoxを計算
				Vec2 BBoxX = { Min({v1.x, v2.x, v3.x}), Max({v1.x, v2.x, v3.x}) };
				Vec2 BBoxY = { Min({v1.y, v2.y, v3.y}), Max({v1.y, v2.y, v3.y}) };

				// 辺の生成
				double dx12 = v1.x - v2.x, dy12 = v1.y - v2.y;
				double dx23 = v2.x - v3.x, dy23 = v2.y - v3.y;
				double dx31 = v3.x - v1.x, dy31 = v3.y - v1.y;

				for (int x = BBoxX.x; x <= BBoxX.y; x++)
				{
					for (int y = BBoxY.x; y <= BBoxY.y; y++)
					{
						Vec2 p{ x,y };
						if (IsInside(p, v1.xy(), dx12, dy12) == false) { continue; }
						if (IsInside(p, v2.xy(), dx23, dy23) == false) { continue; }
						if (IsInside(p, v3.xy(), dx31, dy31) == false) { continue; }

						m_renderImage[y][x] = Palette::White;
					}
				}
			}
			*/
			// Pattern2: Surtherland-Hodgemanによるクリッピング
			{
				bool isIncorrectRegion =
					IsCorrectRegionVertex(m_renderImage.width() - 1, m_renderImage.height() - 1, v1.xy()) == false ||
					IsCorrectRegionVertex(m_renderImage.width() - 1, m_renderImage.height() - 1, v2.xy()) == false ||
					IsCorrectRegionVertex(m_renderImage.width() - 1, m_renderImage.height() - 1, v3.xy()) == false;

				if (isIncorrectRegion)
				{
					// Clip用のLineをWindowから生成
					Array<Vec2> clips;
					clips.push_back({ 0, 0 });
					clips.push_back({ m_renderImage.width() - 1, 0 });
					clips.push_back({ m_renderImage.width() - 1, m_renderImage.height() - 1 });
					clips.push_back({ 0, m_renderImage.height() - 1 });

					// Subject用のLineをTriangleから生成
					Array<Vec2> subjects;
					subjects.push_back(v1.xy());
					subjects.push_back(v2.xy());
					subjects.push_back(v3.xy());

					Array<Vec2> newVertices = SurtherlandHodgeman(clips, subjects);

					if (newVertices.isEmpty())
					{
						continue;
					}

					// BBoxを計算
					Vec2 BBoxX = { Min({v1.x, v2.x, v3.x}), Max({v1.x, v2.x, v3.x}) };
					Vec2 BBoxY = { Min({v1.y, v2.y, v3.y}), Max({v1.y, v2.y, v3.y}) };
					newVertices.push_back(newVertices[0]);
					for (int x = BBoxX.x; x <= BBoxX.y; x++)
					{
						for (int y = BBoxY.x; y <= BBoxY.y; y++)
						{
							bool isCorrect = true;
							for (int i = 0; i < newVertices.size() - 1; i++)
							{
								Vec2 p{ x,y };
								Vec2 dist = newVertices[i] - newVertices[i+1];
								if (IsInside(p, newVertices[i], dist.x, dist.y) == false) { isCorrect = false; break; }
							}

							if (isCorrect)
							{
								m_renderImage[y][x] = Palette::White;
							}
						}
					}

				}
				else
				{
					// BBoxを計算
					Vec2 BBoxX = { Min({v1.x, v2.x, v3.x}), Max({v1.x, v2.x, v3.x}) };
					Vec2 BBoxY = { Min({v1.y, v2.y, v3.y}), Max({v1.y, v2.y, v3.y}) };

					// 辺の生成
					double dx12 = v1.x - v2.x, dy12 = v1.y - v2.y;
					double dx23 = v2.x - v3.x, dy23 = v2.y - v3.y;
					double dx31 = v3.x - v1.x, dy31 = v3.y - v1.y;

					for (int x = BBoxX.x; x <= BBoxX.y; x++)
					{
						for (int y = BBoxY.x; y <= BBoxY.y; y++)
						{
							Vec2 p{ x,y };
							if (IsInside(p, v1.xy(), dx12, dy12) == false) { continue; }
							if (IsInside(p, v2.xy(), dx23, dy23) == false) { continue; }
							if (IsInside(p, v3.xy(), dx31, dy31) == false) { continue; }

							m_renderImage[y][x] = Palette::White;
						}
					}
				}
			}

			// Pattern3: 必要なピクセルの場合は起動
			/*
			{
				// BBoxを計算
				Vec2 BBoxX = { Min({v1.x, v2.x, v3.x}), Max({v1.x, v2.x, v3.x}) };
				Vec2 BBoxY = { Min({v1.y, v2.y, v3.y}), Max({v1.y, v2.y, v3.y}) };

				// 辺の生成
				double dx12 = v1.x - v2.x, dy12 = v1.y - v2.y;
				double dx23 = v2.x - v3.x, dy23 = v2.y - v3.y;
				double dx31 = v3.x - v1.x, dy31 = v3.y - v1.y;

				for (int x = BBoxX.x; x <= BBoxX.y; x++)
				{
					for (int y = BBoxY.x; y <= BBoxY.y; y++)
					{
						Vec2 p{ x,y };
						if (IsCorrectRegionVertex(m_renderImage.width() - 1, m_renderImage.height() - 1, p) == false) { continue; }
						if (IsInside(p, v1.xy(), dx12, dy12) == false) { continue; }
						if (IsInside(p, v2.xy(), dx23, dy23) == false) { continue; }
						if (IsInside(p, v3.xy(), dx31, dy31) == false) { continue; }

						m_renderImage[y][x] = Palette::White;
					}
				}
			}
			*/
		}
	}

	double SurtherlandHodgemanApp::CalculateEdge(const Vec2& p, const Vec2& origin, const float dx, const float dy) const
	{
		return (p.x - origin.x) * dy - (p.y - origin.y) * dx;
	}

	bool SurtherlandHodgemanApp::IsInside(const Vec2& p, const Vec2& origin, const double dx, const double dy) const
	{
		double edge = CalculateEdge(p, origin, dx, dy);
		if (Abs(edge) < 0.00001) { return true; }

		return 0.0 < edge;
	}

	Array<Vec2> SurtherlandHodgemanApp::SurtherlandHodgeman(const Array<Vec2>& clips, const Array<Vec2>& subjects)
	{
		Array<Vec2> vertices = subjects;
		Array<Vec2> inputs;

		auto Intersection = [](Vec2 cs, Vec2 ce, Vec2 ss, Vec2 se)
			{
				return ((ss - se) * Cross(cs, ce) - (cs - ce) * Cross(ss, se)) *
					(1.0 / Cross(cs - ce, ss - se));
			};

		auto Inside = [](Vec2 p, Vec2 start, Vec2 end)
			{
				return (Cross(start - end, p) + Cross(end, start)) < 0.0;
			};

		Vec2 cs = clips[clips.size() - 1];

		for (const auto& ce : clips)
		{
			inputs.clear();
			inputs = vertices; vertices.clear();

			// sizeが0の場合はVertexが矩形外なので打ち切り
			if (inputs.size() == 0)
			{
				return vertices;
			}

			Vec2 ss = inputs[inputs.size() - 1];
			for (const auto& se : inputs)
			{
				if (Inside(se, cs, ce) == true)
				{
					// 終点が内
					if (Inside(ss, cs, ce) == false)
					{
						vertices.push_back(Intersection(cs, ce, ss, se));
					}

					// 両方内側 or 終点が内
					vertices.push_back(se);
				}
				else if (Inside(ss, cs, ce) == true)
				{
					//　始点が内
					vertices.push_back(Intersection(cs, ce, ss, se));
				}

				ss = se;
			}

			cs = ce;
		}

		return vertices;
	}
}
