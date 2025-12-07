#include "GSVertexApp.h"
#include "../../Common/Camera.h"
#include "../../Common/Mesh.h"
#include "../../Common/Model.h"
#include "../../Common/Primitive.h"
#include "../../Common/Scene.h"
#include "../../Common/SceneNode.h"
#include "../../Common/Vertex.h"
#include "../../Common/DrawMethod/BresenhamDrawMethod.h"
#include "../../Common/DrawMethod/PointDrawMethod.h"

namespace TerakoyaRenderer::GS
{
	void GSVertexApp::Prepare()
	{
		// GSのデータを構築
		LoadPly(U"cactus_splat3_30kSteps_142k_splats.ply");

		m_camera->SetUp(Vec3::Up());
		m_camera->SetOrigin(Vec3{ 0.0f, -0.5f, 4.0f });
		m_camera->SetLookAt(Vec3{ 0.0f, -0.5f, 0.0f });
		m_camera->SetNear(0.01);
		m_camera->SetFar(1000);
	}

	void GSVertexApp::Render()
	{
		auto Mat3_3FromMat4_4 = [](const Mat4x4& mat)
			{
				return Mat3x3
				{
					mat.value.r[0].m128_f32[0], mat.value.r[0].m128_f32[1], mat.value.r[0].m128_f32[2],
					mat.value.r[1].m128_f32[0], mat.value.r[1].m128_f32[1], mat.value.r[1].m128_f32[2],
					mat.value.r[2].m128_f32[0], mat.value.r[2].m128_f32[1], mat.value.r[2].m128_f32[2],
				};
			};

		auto Mat3_3Transposed = [](const Mat3x3& mat)
			{
				return Mat3x3
				{
					mat._11, mat._21, mat._31,
					mat._12, mat._22, mat._32,
					mat._13, mat._23, mat._33,
				};
			};

		auto Mat3_3Mul = [](const Mat3x3& lhs, const Mat3x3& rhs)
			{
				return Mat3x3
				{
					// 1row
					lhs._11 * rhs._11 + lhs._12 * rhs._21 + lhs._13 * rhs._31,
					lhs._11 * rhs._12 + lhs._12 * rhs._22 + lhs._13 * rhs._32,
					lhs._11 * rhs._13 + lhs._12 * rhs._23 + lhs._13 * rhs._33,
					// 2row
					lhs._21 * rhs._11 + lhs._22 * rhs._21 + lhs._23 * rhs._31,
					lhs._21 * rhs._12 + lhs._22 * rhs._22 + lhs._23 * rhs._32,
					lhs._21 * rhs._13 + lhs._22 * rhs._23 + lhs._23 * rhs._33,
					// 3row
					lhs._31 * rhs._11 + lhs._32 * rhs._21 + lhs._33 * rhs._31,
					lhs._31 * rhs._12 + lhs._32 * rhs._22 + lhs._33 * rhs._32,
					lhs._31 * rhs._13 + lhs._32 * rhs._23 + lhs._33 * rhs._33,
				};
			};

		auto viewMat = m_camera->GetViewMatrix();
		auto projMat = m_camera->GetProjMatrix();

		double ff = 1.0, nn = 0.0;
		Mat4x4 viewportMat = Mat4x4::Set
		(
			::Scene::Size().x / 2.0, 0, 0, ::Scene::Size().x / 2.0 + 0.0,
			0, ::Scene::Size().y / 2.0, 0.0, ::Scene::Size().y / 2.0 + 0.0,
			0, 0, (ff - nn) / 2.0, (ff + nn) / 2.0,
			0, 0, 0, 1
		);

		// 頂点側を変形しておく
		for (const auto& v : m_gsData.m_positions)
		{
			auto tempV = DirectX::XMVector4Transform(DirectX::XMVectorSet(v.x, v.y, v.z, v.w), viewMat.transposed());

			// Viewに移した位置は後で使うので保持
			m_gsCalculateData.m_viewPos.push_back(Vec4{ tempV.m128_f32[0], tempV.m128_f32[1], tempV.m128_f32[2], tempV.m128_f32[3] });

			tempV = DirectX::XMVector4Transform(tempV, projMat);

			// 正規化デバイス座標へ
			float w = Abs(tempV.m128_f32[3]);
			tempV.m128_f32[0] = tempV.m128_f32[0] / w;
			tempV.m128_f32[1] = tempV.m128_f32[1] / w;
			tempV.m128_f32[2] = tempV.m128_f32[2] / w;
			tempV.m128_f32[3] = 1.0f;

			tempV = DirectX::XMVector4Transform(tempV, viewportMat.transposed());
			auto vv = Vec4{ tempV.m128_f32[0], tempV.m128_f32[1], tempV.m128_f32[2], tempV.m128_f32[3] };

			// Viewport座標のものを用意
			m_gsCalculateData.m_us.push_back(vv);

			if (IsCorrectRegionVertex(m_renderImage.width() - 1, m_renderImage.height() - 1, vv.xy()))
			{
				m_renderImage[static_cast<int>(vv.y)][static_cast<int>(vv.x)] = Palette::White;
			}
		}
		// pngを出力しておく
		m_renderImage.savePNG(FilePathView(U"Dump/Result_Sample.png"));

		// f= 0.5 * resolution /Tan(fov/2)より焦点距離を事前計算
		float fov = m_camera->GetFov();
		double tan_fov = Math::Tan(0.5 * fov);
		double fx = 0.5 * ::Scene::Size().x / tan_fov;
		double fy = 0.5 * ::Scene::Size().x / tan_fov;

		for (int i = 0; i < m_gsData.m_rotations.size(); i++)
		{
			// まず3D共分散を計算
			Mat4x4 RS =  m_gsData.m_rotations[i] * m_gsData.m_scales[i];
			Mat4x4 transRS = RS.transposed();
			Mat3x3 sigma3D3_3 = Mat3_3FromMat4_4(RS * transRS);

			Mat3x3 W = Mat3_3FromMat4_4(viewMat);

			// WJを計算
			Vec4 v = m_gsCalculateData.m_viewPos[i];
			Mat3x3 J{
				static_cast<float>(fx / v.z),  0, -static_cast<float>(fx * v.x / (v.z * v.z)),
				0,  static_cast<float>(fy / v.z), -static_cast<float>(fy * v.y / (v.z * v.z)),
				0,0,0
			};

			Mat3x3 T = Mat3_3Mul(J , W);
			Mat3x3 transT = Mat3_3Transposed(T);
			Mat3x3 Result = Mat3_3Mul(T, sigma3D3_3);
			Result = Mat3_3Mul(Result, transT);
			m_gsCalculateData.m_sigmaMatrices.push_back(Result);
		}

		// SHを計算
		for (int i = 0; i < m_gsData.m_rotations.size(); i++)
		{
			Vec3 direction = m_gsData.m_positions[i].xyz() - m_camera->GetOrigin();
			direction.normalize();
			double x = direction.x, y = direction.y, z = direction.z;
			double xy = x * y, xz = x * z, yz = y * z, xx = x * x, yy = y * y, zz = z * z;

			Vec3 color =
				// l=0
				0.5 * Vec3::One()
				+ 0.28209479177387814 * m_gsData.m_dcs[i].xyz() //float3(f_dc_0, f_dc_1, f_dc_2)
				/*
				// l=1
				- 0.48860251190291987 * y * m_gsData.m_shCoefficients[0][i].xyz()
				+ 0.48860251190291987 * z * m_gsData.m_shCoefficients[1][i].xyz()
				- 0.48860251190291987 * x * m_gsData.m_shCoefficients[2][i].xyz()
				// l=2
				+ 1.0925484305920792 * xy * m_gsData.m_shCoefficients[3][i].xyz()
				- 1.0925484305920792 * yz * m_gsData.m_shCoefficients[4][i].xyz()
				+ 0.31539156525252005 * (2.0 * zz - xx - yy) * m_gsData.m_shCoefficients[5][i].xyz()
				- 1.0925484305920792 * xz * m_gsData.m_shCoefficients[6][i].xyz()
				+ 0.54627421529603959 * (xx - yy) * m_gsData.m_shCoefficients[7][i].xyz()
				// l=3
				- 0.59004358992664352 * y * (3.0 * xx - yy) * m_gsData.m_shCoefficients[8][i].xyz()
				+ 2.8906114426405538 * xy * z * m_gsData.m_shCoefficients[9][i].xyz()
				- 0.45704579946446572 * y * (4.0 * zz - xx - yy) * m_gsData.m_shCoefficients[10][i].xyz()
				+ 0.3731763325901154 * z * (2.0 * zz - 3.0 * xx - 3.0 * yy) * m_gsData.m_shCoefficients[11][i].xyz()
				- 0.45704579946446572 * x * (4.0 * zz - xx - yy) * m_gsData.m_shCoefficients[12][i].xyz()
				+ 1.4453057213202769 * z * (xx - yy) * m_gsData.m_shCoefficients[13][i].xyz()
				- 0.59004358992664352 * x * (xx - 3.0 * yy) * m_gsData.m_shCoefficients[14][i].xyz()*/;

			m_gsCalculateData.m_colors.push_back(Vec4{ color, 1.0 });
		}

		// Z-Sort
		Array<std::pair<double, int>> zSortList;
		for (int i = 0; i < m_gsData.m_positions.size(); i++)
		{
			zSortList.push_back({ m_gsCalculateData.m_viewPos[i].z, i });
		}
		std::sort(zSortList.begin(), zSortList.end(),
			[](const std::pair<double, int>& a, const std::pair<double, int>& b)
		{
			return a.first > b.first;
		});

		// 色を算出
		Array<Array<double>> alphaList;
		Array<Array<Vec4>> resultList;
		alphaList.resize(m_renderImage.width()); resultList.resize(m_renderImage.width());
		for (auto& alphaData : alphaList){ alphaData.resize(m_renderImage.height(), 1.0); }
		for (auto& resultData : resultList){ resultData.resize(m_renderImage.height(), Vec4{0,0,0,1}); }

		// 撮影用意
		VideoWriter videoWriter_01{ U"Dump/Test_01.mp4", ::Scene::Size(), 60 };
		VideoWriter videoWriter_02{ U"Dump/Test_02.mp4", ::Scene::Size(), 30 };

		int count = 0;
		for (const std::pair<double, int>& zSortData : zSortList)
		{
			// 2x2でやってみる
			const int index = zSortData.second;
			auto matrix = m_gsCalculateData.m_sigmaMatrices[index];
			double determinant = 1.0 / (matrix._11 * matrix._22 - matrix._12 * matrix._12);
			double m00 = (matrix._22) * determinant;
			double m01 = -matrix._12 * determinant;
			double m11 = (matrix._11) * determinant;

			for (int x = 0; x < m_renderImage.width(); x++)
			{
				for (int y = 0; y < m_renderImage.height(); y++)
				{
					const double alpha = alphaList[x][y];
					// あまりに小さい場合は寄与が少ないので打ち切り
					if (alpha <= 1.0e-10)
					{
						continue;
					} 

					const Vec2 pixelPos{ x,y };

					// Opacityを計算
					Vec2 d = m_gsCalculateData.m_us[index].xy() - pixelPos;

					double opacity = -0.5 *
						(m00 * d.x * d.x + m11 * d.y * d.y + 2.0 * m01 * d.x * d.y);
					opacity = Exp(opacity) * m_gsData.m_opacities[index];

					if (IsNaN(opacity) || isinf(opacity)) { continue; }

					// 色を算出して足し合わせる
					resultList[x][y] += opacity * m_gsCalculateData.m_colors[index] * alpha;

					// 次のOpacityを減衰させるように
					alphaList[x][y] *= (1.0 - opacity);
				}
			}

			count++;

			if (count % 100 == 0)
			{
				for (int x = 0; x < m_renderImage.width(); x++)
				{
					for (int y = 0; y < m_renderImage.height(); y++)
					{
						m_renderImage[y][x] = ColorF{ resultList[x][y].xyz()};
					}
				}

				// pngを出力しておく
				videoWriter_01.writeFrame(m_renderImage);

				if (count % 1000 == 0)
				{
					videoWriter_02.writeFrame(m_renderImage);
					m_renderImage.savePNG(FilePathView(U"Dump/Result_" + Format(count) + U".png"));
				}
			}
		}

		// 最終結果!!
		for (int x = 0; x < m_renderImage.width(); x++)
		{
			for (int y = 0; y < m_renderImage.height(); y++)
			{
				m_renderImage[y][x] = ColorF{ resultList[x][y].xyz() };
			}
		}
		videoWriter_01.writeFrame(m_renderImage);
		videoWriter_02.writeFrame(m_renderImage);
		videoWriter_01.close(); videoWriter_02.close();

		// 反映
		m_renderTexture.fill(m_renderImage);

		// いらないデータをリセットしておく
		m_gsCalculateData.Clear();
	}

	void GSVertexApp::LoadPly(const String& path)
	{
		BinaryReader binaryReader;
		binaryReader.open(path);

		char c; String s; unsigned int v;

		// 頂点数読み込みと最終位置を確定
		while (true)
		{
			binaryReader.read(&c, 1);

			if (c == 0x0a || c == 0x20)
			{
				if (s == U"vertex")
				{
					s.clear();

					// 数値を読み込む
					while (true)
					{
						binaryReader.read(&c, 1);
						if (c == 0x0a || c == 0x20)
						{
							v = Parse<unsigned int>(s);
							break;
						}

						s += c;
					}
				}

				// 最後尾
				if (s == U"end_header")
				{
					break;
				}

				s.clear();
			}
			else
			{
				s += c;
			}
		}

		float x, y, z, w;
		m_gsData.m_shCoefficients.resize(15);
		for (int k = 0; k < v; k++)
		{
			// position
			binaryReader.read(&x, 4);
			binaryReader.read(&y, 4);
			binaryReader.read(&z, 4);
			m_gsData.m_positions.push_back(Vec4{ x,y,z,1 });

			// dc
			binaryReader.read(&x, 4);
			binaryReader.read(&y, 4);
			binaryReader.read(&z, 4);
			m_gsData.m_dcs.push_back(Vec4{ x,y,z,1 });

			// rest
			for (int i = 0; i < 15; i++)
			{
				binaryReader.read(&x, 4);
				binaryReader.read(&y, 4);
				binaryReader.read(&z, 4);
				m_gsData.m_shCoefficients[i].push_back(Vec4{ x,y,z,1 });
			}

			// opacity
			binaryReader.read(&x, 4);
			m_gsData.m_opacities.push_back(1.0 / (1.0 + Exp(-x)));

			// scale
			binaryReader.read(&x, 4);
			binaryReader.read(&y, 4);
			binaryReader.read(&z, 4);
			m_gsData.m_scales.push_back(Mat4x4::Scale(Vec3{ Exp(x),Exp(y),Exp(z) }));

			//rot
			binaryReader.read(&w, 4);
			binaryReader.read(&x, 4);
			binaryReader.read(&y, 4);
			binaryReader.read(&z, 4);
			Quaternion q;
			q.set(x, y, z, w);
			// 回転行列に変換,ただし転置されてるので、転置を入れて正しくしておく
			m_gsData.m_rotations.push_back(Mat4x4{ q.normalized() }.transposed());
		}
	}
}
