#pragma once
#include <Siv3D.hpp>

void InverseMethodSample()
{
	Scene::SetBackground(Palette::White);

	std::random_device rd;
	std::mt19937 mt(rd());

	// 指数分布は pdf = lambda * exp(-lambda * x)
	// 累積分布関数は F = 1 - exp(-lambda * x)
	std::uniform_real_distribution<double> uniformValue(0.0, 0.99999999); // 一様分布[0,1]

	// 今回はlambda=1でやってみる
	const double lambda = 1.0;
	const int sampleNum = 10000000;

	// 累積分布関数の逆関数はF^(-1) = - (1 / lambda) * log(1 - u)
	auto InverseOfCdf = [&]()
		{return -(1 / lambda) * log(1 - uniformValue(mt)); };

	Array<double> samples;
	for (int i = 0; i < sampleNum; i++)
	{
		samples.push_back(InverseOfCdf());
	}

	// ヒストグラム毎に分ける
	const int histogramBin = 150;
	double histPerInterval = (5.0 - 0.0) / histogramBin; // 区間を分割
	Array<int> histogramBins(histogramBin, 0);

	// 各binに分ける
	for (int i = 0; i < sampleNum; i++)
	{
		double value = samples[i];
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
			auto v = startPoint + Vec2{ RectX * count, RectY *(1.0 - weight)};
			auto s = Vec2{ RectX, RectY * weight };
			RectF{ v, s }.draw(Palette::Blue);
			count++;
		}
	}
}
