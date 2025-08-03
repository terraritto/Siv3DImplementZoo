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
		auto r1 = RandomVec3(); r1.x = Abs(r1.x); r1.y = Abs(r1.y); r1.z = Abs(r1.z);
		auto r2 = RandomVec3(); r2.x = Abs(r2.x); r2.y = Abs(r2.y); r2.z = Abs(r2.z);
		auto v = Sqrt(-2.0 * T * Log(r1)) * Cos(Math::TwoPi * r2);
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

					// 各軸での境界計算
					auto AdjustSign = [](double& dist, float L)
						{
							if (Abs(dist) > 0.5 * L)
							{
								// 境界のL/2を超えてたら、逆方向になるように
								// 境界を調整する
								dist -= Sign(dist) * L;
							}
						};
					AdjustSign(diff.x, L);
					AdjustSign(diff.y, L);
					AdjustSign(diff.z, L);

					double distance = Sqrt(diff.dot(diff));

					// 力を計算
					double sr = 1 / distance;
					double ijForce = 24.0 * (2.0 * Pow(sr, 13) - Pow(sr, 7));
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

	auto forceList = CalculateForce();

	constexpr double h = 0.001;
	constexpr double h2 = h * 0.5;
	constexpr double hsq = h * h;

	int count = 0;
	while (System::Update())
	{
		// 更新
		{
			// 速度ベルレ法で計算していく
			for (int i = 0; auto& position : particlePositions)
			{
				// r(t+h) = r(t) + v(t)h + F(t)/(2m)*h^2
				position = position + h * particleVelocities[i] +
					forceList[i] * 0.5 * hsq;

				// 境界条件
				auto Boundary = [](double& pos, double L)
					{
						if (Abs(pos) > L * 0.5)
						{
							pos -= Sign(pos) * L;
						}
					};
				Boundary(position.x, L);
				Boundary(position.y, L);
				Boundary(position.z, L);

				// v_d = v(t) + F(t)/(2m)*h
				particleVelocities[i] += h2 * forceList[i];

				i++;
			}

			// 強さを更新
			forceList.clear();
			forceList = CalculateForce();

			for (int i = 0; auto& velocity : particleVelocities)
			{
				// v(t+h) = v_d + F(t+h)/(2m)*h
				velocity += h2 * forceList[i];
				i++;
			}
		}

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

			double k = 0.0;
			for (auto& v : particleVelocities)
			{
				k += v.dot(v);
			}
			k = k / particleNum;
			double T = k / (3.0 * particleNum);


			ClearPrint();
			Print << U"温度: " << T;
			Print << U"試行回数: " << count;
			count++;
		}
	}
}
