#pragma once
#include "../PRML.h"

void UniformSample2()
{
	Scene::SetBackground(Palette::White);

	std::shared_ptr<SimpleGrid> grid = std::make_shared<SimpleGrid>();

	grid->SetXGridRegion(Vec2(0.0f, 1.0f));
	grid->SetYGridRegion(Vec2(0.0f, 0.2f));
	grid->SetGridStep(Vec2(0.1f, 0.1f));

	std::shared_ptr<UniformPdfFunction> uniform = std::make_shared<UniformPdfFunction>(grid, 0.0, 1.0);
	std::shared_ptr<FunctionBase> func = std::make_shared<FunctionBase>(grid);

	const int sample = 100000; const int count = 10;
	// Eigen::MatrixXd result = uniform->GetSample(sample);
	// Eigen::MatrixXd result = (uniform->GetSample(sample) + uniform->GetSample(sample)) * 0.5f;
	Eigen::MatrixXd result;
	for (int i = 0; i < count; i++)
	{
		result = i == 0 ? uniform->GetSample(sample) : (result + uniform->GetSample(sample));
	}

	result /= count;

	func->ConstructHistogram(result, 40);

	while (System::Update())
	{
		func->Draw();
	}
}
