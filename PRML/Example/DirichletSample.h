#pragma once
#include "../PRML.h"

void DirichletSample()
{
	Scene::SetBackground(Palette::White);

	std::shared_ptr<SimpleGrid> grid = std::make_shared<SimpleGrid>();

	grid->SetXGridRegion(Vec2(0.0f, 1.0f));
	grid->SetYGridRegion(Vec2(0.0f, 6.0f));
	grid->SetGridStep(Vec2(0.1f, 0.3f));

	Eigen::MatrixXd alpha(3, 1); alpha.setOnes();
	std::shared_ptr<DirichletPdfFunction> dirichlet = std::make_shared<DirichletPdfFunction>(grid, alpha);
	std::shared_ptr<CategoricalPdfFunction> func = std::make_shared<CategoricalPdfFunction>(grid, dirichlet);

	Eigen::MatrixXd result = func->GetAlpha();
	Print << U"[" << result(0, 0) << U", " << result(1, 0) << U", " << result(2, 0) << U"]";

	Eigen::MatrixXd m(3, 3);
	m(0, 0) = 1; m(1, 0) = 0; m(2, 0) = 0;
	m(0, 1) = 1; m(1, 1) = 0; m(2, 1) = 0;
	m(0, 2) = 0; m(1, 2) = 1; m(2, 2) = 0;
	func->Fit(m);

	result = func->GetAlpha();
	Print << U"[" << result(0, 0) << U", " << result(1, 0) << U", " << result(2, 0) << U"]";

	while (System::Update())
	{
	}
}
