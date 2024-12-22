#pragma once
#include "../PRML.h"

void RidgeRegressionSample()
{
	Scene::SetBackground(Palette::White);

	std::shared_ptr<SimpleGrid> grid = std::make_shared<SimpleGrid>();
	std::shared_ptr<SinFunction> func = std::make_shared<SinFunction>(grid);
	std::shared_ptr<ToySinFunction> func2 = std::make_shared<ToySinFunction>(grid, 0.0, 0.25);
	std::unique_ptr<PolynomialFeature> poly = std::make_unique<PolynomialFeature>(9);
	std::unique_ptr<RidgeRegression> regression = std::make_unique<RidgeRegression>(0.001);

	// Toyデータの生成
	func->ConstructLine(); func2->ConstructCircle(20);

	// 実際に予測
	poly->SetDegree(9);

	// Polynomialの計算をしておく
	auto trainData = poly->Transform<double>(func2->GetTrainX());
	auto testData = poly->Transform<double>(func->GetTrainX());

	// 回帰
	regression->fit(trainData, func2->GetTrainY());
	Array<double> predict = regression->Predict(testData);

	// グラフを作成
	std::shared_ptr<FunctionBase> predictFunc = std::make_shared<FunctionBase>(grid);
	predictFunc->CustomConstructLine(predict);

	while (System::Update())
	{
		grid->Draw();
		func->Draw(Palette::Green);
		func2->Draw();

		predictFunc->Draw();
	}
}
