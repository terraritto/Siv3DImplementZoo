#pragma once
#include "../PRML.h"

// Regularized least squares
void RegularizedSample()
{
	Scene::SetBackground(Palette::White);

	std::shared_ptr<SimpleGrid> grid = std::make_shared<SimpleGrid>();
	grid->SetXGridRegion(Vec2(0.0f, 7.0f));

	constexpr int Sample = 80;

	std::shared_ptr<SinFunction> func = std::make_shared<SinFunction>(grid);
	std::shared_ptr<ToySinFunction> func2 = std::make_shared<ToySinFunction>(grid, 0.0, 0.25);

	//std::unique_ptr<PolynomialFeature> poly = std::make_unique<PolynomialFeature>(8);
	std::unique_ptr<GaussianFeature> poly = std::make_unique<GaussianFeature>(MakeGrid1D(8, 0.0, 7.0), 1.0);
	std::unique_ptr<RidgeRegression> regression = std::make_unique<RidgeRegression>(0.001);

	// Toyデータの生成
	func->ConstructLine(); func2->ConstructCircle(Sample);

	// Polynomialの計算をしておく
	auto trainData = poly->Transform<double>(ConvertMatrix1DFromArray(func2->GetTrainX()));
	auto testData = poly->Transform<double>(ConvertMatrix1DFromArray(func->GetTrainX()));
	//auto trainData = poly->Transform<double>(func2->GetTrainX());
	//auto testData = poly->Transform<double>(func->GetTrainX());

	// 回帰
	regression->fit(trainData, func2->GetTrainY());
	Array<double> predict = regression->Predict(testData);

	// グラフを作成
	std::shared_ptr<FunctionBase> predictFunc = std::make_shared<FunctionBase>(grid);
	predictFunc->CustomConstructLine(predict);

	while (System::Update())
	{
		func->Draw();
		func2->Draw(Palette::Blue);
		predictFunc->Draw(Palette::Green);
	}
}
