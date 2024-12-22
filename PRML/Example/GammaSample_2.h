#pragma once
#include "../PRML.h"

void GammaSample2()
{
	Scene::SetBackground(Palette::White);

	std::shared_ptr<SimpleGrid> grid = std::make_shared<SimpleGrid>();
	std::shared_ptr<GammaPdfFunction> gamma = std::make_shared<GammaPdfFunction>(grid, 1.0, 1.0);
	std::shared_ptr<GaussianPdfFunction> model = std::make_shared<GaussianPdfFunction>(grid, ConvertMatrixFromNumeric<double>(0.0), gamma);

	Print << U"Gamma[a: " << gamma->GetA() << U", b: " << gamma->GetB() << U"]";

	Eigen::MatrixXd data = MakeNormalDistributionAxis(100, 0.0, 1.414, 1);
	model->Fit(data);
	Print << U"Gamma[a: " << gamma->GetA() << U", b: " << gamma->GetB() << U"]";

	while (System::Update())
	{
	}
}
