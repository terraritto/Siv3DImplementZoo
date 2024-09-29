#pragma once
#include <Siv3D.hpp>

void BoxMullerSample()
{
	Scene::SetBackground(Palette::White);

	std::random_device rd;
	std::mt19937 mt(rd());

	std::uniform_real_distribution<double> uniformValue(0.0, 0.99999999); // 一様分布[0,1]

	const int sampleNum = 5000000;

	// Z_0 = sqrt(-2ln(x_n)) * cos(2 * pi * y_n)
	// Z_0 = sqrt(-2ln(x_n)) * sin(2 * pi * y_n)

	Array<double> samples;
	for (int i = 0; i < sampleNum; i++)
	{
		auto r = Sqrt(-2.0 * Log(uniformValue(mt)));
		auto theta = 2.0 * Math::Pi * uniformValue(mt);
		samples.push_back(r * Cos(theta));
		samples.push_back(r * Sin(theta));
	}

	// ヒストグラム毎に分ける
	const int histogramBin = 150;
	double histPerInterval = 8.0 / histogramBin; // [-4,4]の8の範囲で考える
	Array<int> histogramBins(histogramBin, 0);

	// 各binに分ける
	for (int i = 0; i < sampleNum; i++)
	{
		double value = (samples[i] + 4.0); // [-4, 4] -> [0, 8]に変換
		if (value < 0.0) { continue; } // 0より小さい場合は入れない

		int binIndex = value / histPerInterval;
		// 5.0より小さいもののみで描画する
		if (binIndex < histogramBin)
		{
			histogramBins[binIndex]++;
		}
	}

	// 最大値を抽出
	int maxValue = 0;
	for (auto& bin : histogramBins)
	{
		maxValue = Max(maxValue, bin);
	}

	double RectY = 400;
	double RectX = 5;

	Vec2 startPoint = { 50.0, 50.0 };

	while (System::Update())
	{
		int count = 0;
		for (auto& bin : histogramBins)
		{
			auto weight = (bin / static_cast<double>(maxValue));
			auto v = startPoint + Vec2{ RectX * count, RectY * (1.0 - weight) };
			auto s = Vec2{ RectX, RectY * weight };
			RectF{ v, s }.draw(Palette::Blue);
			count++;
		}
	}
}
