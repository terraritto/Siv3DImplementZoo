#pragma once
#include "../PRML.h"

void BernoulliSample()
{
	Scene::SetBackground(Palette::White);

	std::shared_ptr<SimpleGrid> grid = std::make_shared<SimpleGrid>();

	grid->SetXGridRegion(Vec2(0.0f, 1.0f));
	grid->SetYGridRegion(Vec2(0.0f, 6.0f));
	grid->SetGridStep(Vec2(0.1f, 0.3f));

	std::shared_ptr<BetaPdfFunction> func = std::make_shared<BetaPdfFunction>(grid, 2.0, 2.0);
	std::unique_ptr<BernoulliPdfFunction> func2 = std::make_unique<BernoulliPdfFunction>(grid, func);

	Eigen::MatrixXd m(1, 1);
	m(0, 0) = 1;
	func2->Fit(m);

	func->ConstructLine(); func2->ConstructLine();

	while (System::Update())
	{
		grid->Draw();
		func->Draw(); func2->Draw();
	}
}
