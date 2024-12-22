#pragma once
#include "../PRML.h"

void UniformSample()
{
	Scene::SetBackground(Palette::White);

	std::shared_ptr<SimpleGrid> grid = std::make_shared<SimpleGrid>();

	grid->SetXGridRegion(Vec2(0.0f, 1.0f));
	grid->SetYGridRegion(Vec2(0.0f, 6.0f));
	grid->SetGridStep(Vec2(0.1f, 0.3f));

	std::shared_ptr<BetaPdfFunction> func = std::make_shared<BetaPdfFunction>(grid, 1.0, 1.0);
	std::unique_ptr<BernoulliPdfFunction> func_1 = std::make_unique<BernoulliPdfFunction>(grid);
	std::unique_ptr<BernoulliPdfFunction> func_2 = std::make_unique<BernoulliPdfFunction>(grid, func);

	Eigen::MatrixXd m(1, 1);
	m(0, 0) = 1;
	func_1->Fit(m); func_2->Fit(m);

	Print << U"{} out of 10000 is 1"_fmt(func_1->GetSample(10000));
	Print << U"{} out of 10000 is 1"_fmt(func_2->GetSample(10000));

	while (System::Update())
	{
	}
}
