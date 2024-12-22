#pragma once
#include "../PRML.h"

void StudentsTSample()
{
	Scene::SetBackground(Palette::White);

	std::shared_ptr<SimpleGrid> grid = std::make_shared<SimpleGrid>();

	grid->SetXGridRegion(Vec2(-5.0f, 25.0f));
	grid->SetYGridRegion(Vec2(0.0f, 0.5f));
	grid->SetGridStep(Vec2(0.1f, 0.1f));

	std::shared_ptr<UniformPdfFunction> uniform_01 = std::make_shared<UniformPdfFunction>(grid, 0.0, 1.0);
	std::shared_ptr<UniformPdfFunction> uniform_02 = std::make_shared<UniformPdfFunction>(grid, 20.0, 1.0);
	std::shared_ptr<GaussianPdfFunction> gaussian = std::make_shared<GaussianPdfFunction>(grid);
	std::shared_ptr<StudentsTPdfFunction> students = std::make_shared<StudentsTPdfFunction>(grid);

	std::shared_ptr<FunctionBase> func = std::make_shared<FunctionBase>(grid);

	Eigen::MatrixXd first = uniform_01->GetSample(20);
	Eigen::MatrixXd second = uniform_02->GetSample(3);

	Eigen::MatrixXd result(first.rows() + second.rows(), first.cols());
	result << first, second;

	// Fit系
	gaussian->Fit(result);
	students->Fit(result);

	// 予測系
	int size = grid->GetWidth();
	Eigen::MatrixXd meshData = MakeGrid1D(size, -5.0f, 25.0f);
	Array<double> gaussianResult = ConvertArrayFromMatrix(gaussian->Pdf(meshData));
	Array<double> studentsResult = ConvertArrayFromMatrix(students->Pdf(meshData));

	func->ConstructHistogram(result, 100);
	gaussian->CustomConstructLine(gaussianResult);
	students->CustomConstructLine(studentsResult);

	while (System::Update())
	{
		func->Draw();
		gaussian->Draw();
		students->Draw(Palette::Blue);
	}
}
