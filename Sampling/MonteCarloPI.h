#pragma once
#include <Siv3D.hpp>

void MonteCarloPISample()
{
	Scene::SetBackground(Palette::White);

	int32 insidePointNum = 0;
	int32 totalAttempted = 0;

	std::random_device rd;
	std::mt19937 mt(rd());
	std::uniform_real_distribution<double> pos(-1, 1);

	Array<Vec2> points;
	double estimate;

	auto SphereSample = [&](int sampleNum, bool isStackPoint)
	{
		if (isStackPoint) { points.clear(); }

		for (int i = 0; i < sampleNum; i++)
		{
			Vec2 p{ pos(mt), pos(mt) };
			double distance = p.distanceFrom(Vec2::Zero());

			insidePointNum += static_cast<int32>(distance <= 1.0);
			totalAttempted++;

			if (isStackPoint)
			{
				points.push_back(p);
			}
		}
	};

	auto EstimateSample = [&]()
	{
		return 4.0 * (insidePointNum / static_cast<double>(totalAttempted));
	};

	auto EstimateProcess = [&](int sample, bool isStackPoint = false)
	{
		SphereSample(sample, isStackPoint);
		estimate = EstimateSample();
	};

	EstimateProcess(10);

	while (System::Update())
	{
		ClearPrint();

		if (SimpleGUI::Button(U"10000 Sample", Vec2{ 10, 560 }))
		{
			EstimateProcess(10000);
		}

		if (SimpleGUI::Button(U"100 Sample", Vec2{ 180, 560 }))
		{
			EstimateProcess(100, true);
		}

		if (SimpleGUI::Button(U"1 Sample", Vec2{ 330, 560 }))
		{
			EstimateProcess(1, true);
		}

		for (auto& posTemp : points)
		{
			double distance = posTemp.distanceFrom(Vec2::Zero());
			auto color = distance <= 1.0 ? Palette::Red : Palette::Blue;

			Circle{ Scene::Size().xy() / 2.0 + posTemp * 250.0, 3.0 }.draw(color);
		}

		RectF{ Arg::center(Scene::Size().xy() / 2.0), 500, 500 }.drawFrame(1, Palette::Black);
		Circle{ Scene::Size().xy() / 2.0, 250 }.drawFrame(1, Palette::Black);

		Print << U"Sample: " << totalAttempted;
		Print << U"PI ~= " << U"{:.7f}"_fmt(estimate);
	}
}

