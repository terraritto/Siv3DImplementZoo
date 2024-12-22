#pragma once
#include "..\PRML.h"

void PolynomialCurveFittingSample()
{
	Scene::SetBackground(Palette::White);

	std::shared_ptr<SimpleGrid> grid = std::make_shared<SimpleGrid>();
	std::shared_ptr<SinFunction> func = std::make_shared<SinFunction>(grid);
	std::shared_ptr<ToySinFunction> func2 = std::make_shared<ToySinFunction>(grid, 0.0, 0.25);
	std::unique_ptr<PolynomialFeature> poly = std::make_unique<PolynomialFeature>(9);
	std::unique_ptr<LinearRegression> regression = std::make_unique<LinearRegression>();

	// Toyデータの生成
	func->ConstructLine(); func2->ConstructCircle(40);


	// 実際に予測
	Array<std::shared_ptr<FunctionBase>> predictList;
	Array<Color> colorList{ Palette::Green, Palette::Chocolate, Palette::Bisque, Palette::Lightskyblue, Palette::Azure };

	constexpr size_t Num = 5;
	size_t degree = 5;
	for (size_t i = 0; i < Num; i++)
	{
		poly->SetDegree(degree);

		// Polynomialの計算をしておく
		auto trainData = poly->Transform<double>(func2->GetTrainX());
		auto testData = poly->Transform<double>(func->GetTrainX());

		// 回帰
		regression->fit(trainData, func2->GetTrainY());
		Array<double> predict = regression->Predict(testData);

		// グラフを作成
		std::shared_ptr<FunctionBase> predictFunc = std::make_shared<FunctionBase>(grid);
		predictFunc->CustomConstructLine(predict);
		predictList.emplace_back(predictFunc);

		// degree更新
		degree += 2;
	}


	while (System::Update())
	{
		grid->Draw();
		func->Draw();
		func2->Draw();

		for (size_t i = 0; i < predictList.size(); i++)
		{
			predictList[i]->Draw(colorList[i]);
		}
	}
}
