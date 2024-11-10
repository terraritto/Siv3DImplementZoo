#pragma once
#pragma once
#include <Siv3D.hpp>

void RejectionSample()
{
	Scene::SetBackground(Palette::White);

	std::random_device rd;
	std::mt19937 mt(rd());

	// 指数分布は pdf = lambda * exp(-lambda * x)
	// 正規化定数は const = lambdaなので、f(x)=exp(-lambda * x)
	std::uniform_real_distribution<double> uniformValue(0.0, 0.99999999); // 一様分布[0,1]

	double lambda = 0.5;
	auto expDist = [&lambda](double x) { return Exp(-lambda * x); };

	const double k = 10;
	const int sampleNum = 10000000;

	Array<double> samples;
	for (int i = 0; i < sampleNum; i++)
	{
		// q(x)を一様分布でサンプル
		double proposal = uniformValue(mt);

		// [0, k*q(x)]でuをサンプル
		// 一様分布[0,k * q(x)]
		std::uniform_real_distribution<double> prop(0, k * proposal); 

		double u = prop(mt);

		// 受理の判定
		// u (=kq(x)) < p(q(x))なら受理
		if (u < expDist(proposal))
		{
			samples.push_back(proposal);
		}

	}

	int acceptNum = samples.size();

	// ヒストグラム毎に分ける
	const int histogramBin = 150;
	double histPerInterval = (1.0 - 0.0) / histogramBin; // 区間を分割
	Array<int> histogramBins(histogramBin, 0);

	// 各binに分ける
	for (int i = 0; i < acceptNum; i++)
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
			auto v = startPoint + Vec2{ RectX * count, RectY * (1.0 - weight) };
			auto s = Vec2{ RectX, RectY * weight };
			RectF{ v, s }.draw(Palette::Blue);
			count++;
		}
	}
}
