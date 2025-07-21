#pragma once
#include <Siv3D.hpp>

void MolecularDynamicsSample()
{
	const ColorF backgroundColor = ColorF{ 0.4, 0.6, 0.8 }.removeSRGBCurve();
	const MSRenderTexture renderTexture{ Scene::Size(), TextureFormat::R8G8B8A8_Unorm_SRGB, HasDepth::Yes };

	int particleNum = 27;
	double gap = 1.12;
	double L = Pow(particleNum, 1.0 / 3.0);
	double T = 2.5;

	// カメラ位置を決めておく
	DebugCamera3D camera{ renderTexture.size(), 30_deg, Vec3{ -4.0 * L, 0, 0 } };

	// step1: 粒子の位置を初期化
	Array<Vec3> particlePositions;
	for (int i = 0; i < static_cast<int>(L); i++)
	{
		for (int j = 0; j < static_cast<int>(L); j++)
		{
			for (int k = 0; k < static_cast<int>(L); k++)
			{
				Vec3 position;
				position = Vec3(i,j,k) * gap - Vec3::One() *
					(L - 1) * gap / 2.0
					+ 0.1 * RandomVec3();
				particlePositions.push_back(position);
			}
		}
	}

	// step2: マクスウェル-ボルツマン分布に従って速度を初期化
	Array<Vec3> particleVelocities;
	for (int i = 0; i < particlePositions.size(); i++)
	{
		auto v = Sqrt(-2.0 * T * Log(RandomVec3())) * Cos(Math::TwoPi * RandomVec3());
		particleVelocities.push_back(v);
	}

	// step3: 合力の計算
	auto CalculateForce = [&]()
		{
			// 力を計算する配列を用意
			Array<Vec3> forceList;
			forceList.resize(particleNum, Vec3::Zero());

			for (int i = 0; i < particleNum - 1; i++)
			{
				Vec3 force = forceList[i];

				for (int j = i + 1; j < particleNum; j++)
				{
					// 二点間の差と距離を算出
					Vec3 diff = particlePositions[i] - particlePositions[j];
					double distance = particlePositions[i].distanceFromSq(particlePositions[j]);

					// 力を計算
					double sr2 = 1 / distance;
					double ijForce = 24.0 * (2.0 * Pow(sr2, 13) - Pow(sr2, 7));
					Vec3 fx = ijForce * diff;

					// i側に適用(直接ではない)
					force += fx;

					// j側は直接適用,反作用なので方向は逆
					forceList[j] -= fx;
				}

				// 全部の影響を考えたものを蓄積
				forceList[i] = force;
			}

			return forceList;
		};

	auto forecList = CalculateForce();

	while (System::Update())
	{
		camera.update(2.0);
		Graphics3D::SetCameraTransform(camera);

		// 3D 描画
		{
			const ScopedRenderTarget3D target{ renderTexture.clear(backgroundColor) };

			Box{ Vec3::Zero(), L }.drawFrame();

			for (auto& position : particlePositions)
			{
				Sphere{ position, 0.05 }.draw(Palette::Red);
			}
		}

		// 3D シーンを 2D シーンに描画
		{
			Graphics3D::Flush();
			renderTexture.resolve();
			Shader::LinearToScreen(renderTexture);
		}
	}
}
