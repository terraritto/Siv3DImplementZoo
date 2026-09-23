#pragma once
#include <Siv3D.hpp>

void TakagiCurveSample()
{
	Scene::SetBackground(ColorF{ 1.0, 1.0, 1.0 });

	// 高木曲線用集合
	Array<Array<double>> loopMap;
	Array<double> result;

	double maxValue = 0.0;

	auto MakeTakagi = [&](int loop)
	{
		result.clear();

		// 評価する数
		int evaluate = Pow(2, loop);

		double frequency = Math::TwoPi;
		double period = 1.0 / evaluate;
		double denom = 1.0;

		Array<Array<double>>  trigonometricSet;

		for (int i = 0; i < loop; i++)
		{
			Array<double> temp;

			// 三角波の現在の位置を生成
			// 三角波は arccos(cos(x)) で書ける
			for (int j = 0; j < evaluate; j++)
			{
				double currentPeriod = (j + 1) * period;
				temp.push_back(Math::Acos(Math::Cos(frequency * currentPeriod)) / denom);
			}

			trigonometricSet.push_back(temp);

			// 次の周波数へ
			frequency *= 2.0;
			denom *= 2.0;
		}

		// 最後尾は新しくできた波なので、可視化のために保持しておく
		loopMap.push_back(trigonometricSet.back());

		// 和を計算
		result.resize(trigonometricSet[0].size(), 0);
		for (auto& set : trigonometricSet)
		{
			for (int i = 0; i < result.size(); i++)
			{
				result[i] += set[i];
			}
		}

		// 最大値を計算
		maxValue = *std::max_element(result.begin(), result.end());
	};

	// [0,1]時のLineサイズ
	constexpr int MaxLine = 400;

	// 開始位置
	constexpr Vec2 StartPos = { 200, 500 };

	// Y軸ずらし
	constexpr double YOffset = 20.0;

	int subdivision = 1;

	MakeTakagi(subdivision);

	while (System::Update())
	{
		// 分割が必要なら実行
		if (SimpleGUI::Button(U"分割！！", Vec2{ 10, 10 }))
		{
			subdivision++;
			MakeTakagi(subdivision);
		}

		for (auto& data : loopMap)
		{
			// 元の線を描画
			double loopLength = 1.0 / data.size();
			Vec2 prevPos = StartPos;
			for (int i = 0; i < data.size(); i++)
			{
				Vec2 nextPos = Vec2{ StartPos.x + MaxLine * loopLength * (i + 1), StartPos.y - data[i] / maxValue * MaxLine};
				Line{ prevPos, nextPos }.draw(Palette::Red);

				prevPos = nextPos;
			}
		}

		// 高木曲線の描画
		{
			double loopLength = 1.0 / result.size();
			Vec2 prevPos = StartPos;
			for (int i = 0; i < result.size(); i++)
			{
				Vec2 nextPos = Vec2{ StartPos.x + MaxLine * loopLength * (i + 1), StartPos.y - result[i] / maxValue * MaxLine };
				Line{ prevPos, nextPos }.draw(Palette::Blue);

				prevPos = nextPos;
			}
		}
	}
}
