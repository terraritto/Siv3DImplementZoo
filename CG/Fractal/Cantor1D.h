#pragma once
#include <Siv3D.hpp>

void Cantor1DSample()
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
	constexpr int MaxLine = 700;

	// 開始位置
	constexpr Vec2 StartPos = { 50, 50 };

	// Y軸ずらし
	constexpr double YOffset = 20.0;

	// 分割数
	int subdivision = 0;

	MakeCantor(subdivision);

	while (System::Update())
	{
		// 分割が必要なら実行
		if (SimpleGUI::Button(U"分割！！", Vec2{ 10, 10 }))
		{
			subdivision++;
			MakeCantor(subdivision);
		}

		// 線を描画
		int offset = 0;

		for(auto& contorData : contorSet)
		{
			for (int i = 0; i + 1 <= contorData.size() - 1; i += 2)
			{
				double start = contorData[i], end = contorData[i + 1];
				Vec2 lineStart = StartPos + Vec2{ start * MaxLine, StartPos.y + offset * YOffset };
				Vec2 lineEnd = StartPos + Vec2{ end * MaxLine, StartPos.y + offset * YOffset };
				Line{ lineStart, lineEnd }.draw(Palette::Red);
			}

			offset++;
		}
	}
}
