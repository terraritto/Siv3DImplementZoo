#pragma once
#include <Siv3D.hpp>

// 2次元のカントールの塵
void Cantor2DSample()
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
	constexpr int MaxSize = 500;

	// 開始位置
	constexpr Vec2 StartPos = { 125, 50 };

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

		auto contorData = contorSet.back();
		for (int i = 0; i + 1 <= contorData.size() - 1; i += 2)
		{
			double startX = contorData[i], endX = contorData[i + 1];
			double rectSize = (endX - startX) * MaxSize; // sizeは確定なので保持

			for (int j = 0; j + 1 <= contorData.size() - 1; j+=2)
			{
				double startY = contorData[j];

				Vec2 rectStart = StartPos + Vec2{ startX, startY } * MaxSize;
				RectF{ rectStart, rectSize }.draw(Palette::Red);
			}
		}
	}
}
