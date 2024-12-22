#pragma once
#include "../PRML.h"

// The Bias-Variance Decomposition
void BiasVarianceSample()
{
	Scene::SetBackground(Palette::White);

	std::shared_ptr<SimpleGrid> grid = std::make_shared<SimpleGrid>();
	grid->SetXGridRegion(Vec2(0.0f, 7.0f));
	grid->SetYGridRegion(Vec2(-1.5f, 1.5f));

	constexpr int Sample = 25;

	std::shared_ptr<SinFunction> func = std::make_shared<SinFunction>(grid);
	std::shared_ptr<ToySinFunction> func2 = std::make_shared<ToySinFunction>(grid, 0.0, 0.25);
	func->ConstructLine();

	std::unique_ptr<GaussianFeature> poly = std::make_unique<GaussianFeature>(MakeGrid1D(24, 0.0, 7.0), 0.1);
	auto testData = poly->Transform<double>(ConvertMatrix1DFromArray(func->GetTrainX()));

	Array<std::shared_ptr<FunctionBase>> predictList;


	// Toyデータの生成
	constexpr int Recursion = 100;
	Array<double> alphaList = { 100.0, 10.0, 2.8 };
	Eigen::MatrixXd resultData(grid->GetWidth() + 1, Recursion);

	for (int i = 0; i < Recursion; i++)
	{
		func2->ConstructCircle(Sample);

		// Polynomialの計算をしておく
		auto trainData = poly->Transform<double>(ConvertMatrix1DFromArray(func2->GetTrainX()));

		// 回帰
		std::unique_ptr<BayesianRegression> regression = std::make_unique<BayesianRegression>(alphaList[2], 1.0);
		regression->fit(trainData, func2->GetTrainY());
		Array<double> predict = regression->Predict(testData);

		// グラフを作成
		if (i < 20)
		{
			std::shared_ptr<FunctionBase> predictFunc = std::make_shared<FunctionBase>(grid);
			predictFunc->CustomConstructLine(predict);
			predictList.emplace_back(predictFunc);
		}

		for (int j = 0; j < predict.size(); j++)
		{
			resultData(j, i) = predict[j];
		}
	}

	Array<double> predicted;
	for (int i = 0; i < grid->GetWidth(); i++)
	{
		predicted.push_back(resultData.row(i).mean());
	}
	std::shared_ptr<FunctionBase> result = std::make_shared<FunctionBase>(grid);
	result->CustomConstructLine(predicted);

	while (System::Update())
	{
		func->Draw();
		func2->Draw(Palette::Blue);
		result->Draw(Palette::Orange);

		for (auto& predict : predictList)
		{
			predict->Draw(Palette::Green);
		}
	}
}
