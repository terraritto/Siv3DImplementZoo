#pragma once
#pragma once
#include <Siv3D.hpp>

void AverageMethodSample()
{
	Scene::SetBackground(Palette::Black);

	int32 insidePointNum = 0;
	int32 totalAttempted = 0;
	double averageSum = 0.0;

	std::random_device rd;
	std::mt19937 mt(rd());
	std::uniform_real_distribution<double> xValue(0.00001, 2.0);
	std::uniform_real_distribution<double> yValue(0.0, 4.0);


	double estimateMonteCarlo;
	double estimateAverage;

	auto f = [](double x) {return x * x; }; // x ^ 2

	// y=x^2
	auto Sampleing = [&](int sampleNum)
		{
			for (int i = 0; i < sampleNum; i++)
			{
				// 乱数
				Vec2 p{ xValue(mt), yValue(mt) };

				// モンテカルロは四角形内でサンプリングし、y値がx値を超えてるかで中かを判定
				insidePointNum += static_cast<int32>(p.y <= f(p.x));

				// 平均値法はサンプルの合計を蓄積しておく
				averageSum += f(p.x);

				// 合計値を蓄積
				totalAttempted++;
			}
		};

	auto EstimateSample = [&]()
		{
			estimateMonteCarlo = 8.0 * insidePointNum / totalAttempted; // I ~= SquareArea * n/N
			estimateAverage = 2.0 * averageSum / totalAttempted; // I ~= (b-a)/{N * sum(Y)}(=y_av)
		};

	auto EstimateProcess = [&](int sample)
		{
			Sampleing(sample);
			EstimateSample();
		};

	EstimateProcess(10000000);

	while (System::Update())
	{
		ClearPrint();

		Print << U"Sample: " << totalAttempted;
		Print << U"MonteCarlo: " << U"{:.7f}"_fmt(estimateMonteCarlo) << U" Abs: " << U"{:.7f}"_fmt(Abs(estimateMonteCarlo - 8.0 / 3.0));
		Print << U"Average: " << U"{:.7f}"_fmt(estimateAverage) << U" Abs: " << U"{:.7f}"_fmt(Abs(estimateAverage - 8.0 / 3.0));
		Print << U"GroundTruth: " << U"{:.7f}"_fmt(8.0 / 3.0);
	}
}
