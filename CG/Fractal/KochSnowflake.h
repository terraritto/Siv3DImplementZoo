#pragma once
#include <Siv3D.hpp>

void KochSnowflakeSample()
{
	Scene::SetBackground(ColorF{ 1.0, 1.0, 1.0 });

	// コッホ用集合
	Array<Array<std::pair<Vec2, Vec2>>> kochSet;

	// [0,1]時のLineサイズ
	constexpr int MaxLine = 300;

	// 開始位置
	constexpr Vec2 StartPos = { 250, 200 };

	// Y軸ずらし
	constexpr double YOffset = 20.0;

	// 分割数
	int subdivision = 0;

	auto MakeKoch = [&](int loop)
	{
		kochSet.clear();

		// 初期条件
		kochSet.push_back(
			{
				std::pair<Vec2, Vec2>(StartPos, Vec2{StartPos.x + MaxLine, StartPos.y}),
				std::pair<Vec2, Vec2>(
					Vec2{StartPos.x + MaxLine, StartPos.y},
					Vec2{StartPos.x + MaxLine / 2.0, StartPos.y + MaxLine / 2.0 * Sqrt(3)}),
				std::pair<Vec2, Vec2>(
					Vec2{StartPos.x + MaxLine / 2.0, StartPos.y + MaxLine / 2.0 * Sqrt(3)},
					StartPos),
			});

		auto current = kochSet[0];

		Vec2 defaultDir = Vec2{ 0, 1 };

		for (int i = 0; i < loop; i++)
		{
			Array<std::pair<Vec2, Vec2>> temp;

			// コッホを構築
			for (int j = 0; j < current.size(); j++)
			{
				Vec2 start = current[j].first, end = current[j].second;
				double nextLength = start.distanceFrom(end) / 3.0; // コッホ曲線の1辺の長さ

				// 新しい点を得るのに必要なベクトルを生成
				Vec2 straightLineVec = (end - start).normalized();
				Vec2 pointVec = straightLineVec.rotated(ToRadians(-60.0)); // 60度回転したベクトル生成

				// 点を生成
				Vec2 topPoint = straightLineVec * nextLength + pointVec * nextLength;


				temp.push_back({ start, start + straightLineVec * nextLength });
				temp.push_back({ start + straightLineVec * nextLength, start + topPoint });
				temp.push_back({ start + topPoint,  start + straightLineVec * nextLength * 2.0 });
				temp.push_back({ start + straightLineVec * nextLength * 2.0, end });
			}

			kochSet.push_back(temp);
			current = temp;
		}
	};

	MakeKoch(subdivision);

	while (System::Update())
	{
		// 分割が必要なら実行
		if (SimpleGUI::Button(U"分割！！", Vec2{ 10, 10 }))
		{
			subdivision++;
			MakeKoch(subdivision);
		}

		auto kochData = kochSet.back();
		for (int i = 0; i < kochData.size(); i++)
		{
			auto lineVec = kochData[i];
			Line{ lineVec.first, lineVec.second }.draw(Palette::Red);
		}
	}
}
