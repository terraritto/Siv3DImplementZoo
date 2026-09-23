#pragma once
#include <Siv3D.hpp>

// メンガーのスポンジ
void MengerSample()
{
	Scene::SetBackground(ColorF{ 1.0, 1.0, 1.0 });

	// [0,1]時のLineサイズ
	constexpr int MaxSize = 4;

	auto length = 4.0;

	// メンガー用集合
	Array<Array<Vec3>> mengerSet;

	auto MakeMenger = [&](int loop)
	{
		mengerSet.clear();
		mengerSet.push_back({ Vec3::Zero()});

		length = 4.0;

		Array<Vec3> current = mengerSet[0];

		for (int i = 0; i < loop; i++)
		{
			Array<Vec3> temp;

			length = length / 3.0;

			for (int j = 0; j < current.size(); j++)
			{
				Vec3 start = current[j];

				// 計27個のブロックを保持する
				for (int l = 0; l < 3; l++)
				{
					for (int m = 0; m < 3; m++)
					{
						for (int n = 0; n < 3; n++)
						{

							// 中心は無視
							if (
								l == 1 && m == 1 ||
								l == 1 && n == 1 ||
								m == 1 && n == 1
							)
							{
								continue;
							}

							// 次のサイズ位置を保存
							temp.push_back(start + (Vec3{ l, m, n } - Vec3::One()) * length);
						}
					}
				}
			}

			mengerSet.push_back(temp);
			current = temp;
		}
	};

	// 分割数
	int subdivision = 0;

	MakeMenger(subdivision);

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

			auto mengerData = mengerSet.back();
			for (int i = 0; i < mengerData.size(); i++)
			{
				auto center = mengerData[i];
				Box{ center, length }.draw(Palette::Red);
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
			MakeMenger(subdivision);
		}
	}
}
