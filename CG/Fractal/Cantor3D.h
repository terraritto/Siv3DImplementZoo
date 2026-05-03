#pragma once
#include <Siv3D.hpp>

// 3次元のカントールの塵
void Cantor3DSample()
{
	Scene::SetBackground(ColorF{ 1.0, 1.0, 1.0 });

	// カントール用集合
	Array<Array<double>> contorSet;

	auto MakeCantor = [&](int loop)
	{
		contorSet.clear();
		contorSet.push_back({ 0,1 });

		Array<double> current = contorSet[0];

		for (int i = 0; i < loop; i++)
		{
			Array<double> temp;

			// カントールを構築
			for (int j = 0; j + 1 <= current.size() - 1; j += 2)
			{
				double start = current[j], end = current[j + 1];
				double nextLength = (end - start) / 3.0;

				temp.push_back(start); temp.push_back(start + nextLength);
				temp.push_back(end - nextLength); temp.push_back(end);
			}

			contorSet.push_back(temp);
			current = temp;
		}
	};

	// [0,1]時のLineサイズ
	constexpr int MaxSize = 4;

	// 開始位置
	constexpr Vec3 StartPos = Vec3::Zero() - Vec3::One() * MaxSize / 2.0;

	// 分割数
	int subdivision = 0;

	MakeCantor(subdivision);

	// 3D用
	const ColorF backgroundColor = ColorF{ 0.4, 0.6, 0.8 }.removeSRGBCurve();
	const MSRenderTexture renderTexture{ Scene::Size(), TextureFormat::R8G8B8A8_Unorm_SRGB, HasDepth::Yes };
	DebugCamera3D camera{ renderTexture.size(), 30_deg, Vec3{ 0, 0, -10 }, Vec3::Zero() };

	while (System::Update())
	{
		// 3D描画
		camera.update(2.0);
		Graphics3D::SetCameraTransform(camera);

		// ここで描画、上手く3Dになるように変形していく
		{
			const ScopedRenderTarget3D target{ renderTexture.clear(backgroundColor) };

			auto contorData = contorSet.back();
			for (int i = 0; i + 1 <= contorData.size() - 1; i += 2)
			{
				// X軸
				double startX = contorData[i], endX = contorData[i + 1];
				double rectSize = (endX - startX) * MaxSize; // sizeは確定なので保持
				
				for (int j = 0; j + 1 <= contorData.size() - 1; j += 2)
				{
					// Y軸
					double startY = contorData[j], endY = contorData[j + 1];

					for (int k = 0; k + 1 <= contorData.size() - 1; k += 2)
					{
						// Z軸
						double startZ = contorData[k], endZ = contorData[k + 1];
						Vec3 boxStart = StartPos + Vec3{startX, startY, startZ} * MaxSize;

						Box::FromPoints(boxStart, boxStart + Vec3::One() * rectSize).draw(Palette::Red);
					}
				}
			}
		}

		{
			Graphics3D::Flush();
			renderTexture.resolve();
			Shader::LinearToScreen(renderTexture);
		}

		// 分割が必要なら実行
		if (SimpleGUI::Button(U"分割！！", Vec2{ 10, 10 }))
		{
			subdivision++;
			MakeCantor(subdivision);
		}
	}
}
