#pragma once
#include "../PRML.h"

// Predictive distribution
void PredictiveDistribution()
{
	Scene::SetBackground(Palette::White);

	std::shared_ptr<SimpleGrid> grid = std::make_shared<SimpleGrid>();
	grid->SetXGridRegion(Vec2(0.0, M_PI * 2.0));
	grid->SetYGridRegion(Vec2(-1.0, 1.0));

	constexpr int Sample = 50 - 1;

	std::shared_ptr<SinFunction> func = std::make_shared<SinFunction>(grid);
	std::shared_ptr<ToySinFunction> func2 = std::make_shared<ToySinFunction>(grid, 0.0, 0.25);
	func->ConstructLine(); func2->ConstructCircle(Sample);

	// feature
	std::unique_ptr<GaussianFeature> feature = std::make_unique<GaussianFeature>(MakeGrid1D(24, 0.0, M_PI * 2.0), 0.1);
	auto testData = feature->Transform<double>(ConvertMatrix1DFromArray(func->GetTrainX()));
	auto trainData = feature->Transform<double>(ConvertMatrix1DFromArray(func2->GetTrainX()));
	auto trainX = func2->GetTrainX();
	auto trainY = func2->GetTrainY();

	// 回帰
	std::unique_ptr<BayesianRegression> regression = std::make_unique<BayesianRegression>(8.0, 1.0);

	const Array<std::pair<int, int>> DataList = { {0,1},{1,2},{2,3} ,{3,50} };

	Array<double> lineResult;
	// 計算
	for (int i = 0; i < DataList.size(); i++)
	{
		// fitを行う
		Array<Array<double>> sliceX(trainData.begin() + DataList[i].first, trainData.begin() + DataList[i].second);
		Array<double> sliceY(trainY.begin() + DataList[i].first, trainY.begin() + DataList[i].second);
		regression->fit(sliceX, sliceY);
	}

	lineResult = regression->Predict(testData);

	std::shared_ptr<FunctionBase> func3 = std::make_shared<FunctionBase>(grid);
	std::shared_ptr<FunctionBase> line = std::make_shared<FunctionBase>(grid);
	// --- line側の描画周り start
	// 線の生成
	line->CustomConstructLine(lineResult);

	// scatterの用意
	Array<double> sliceX(trainX.begin(), trainX.begin() + DataList.back().second);
	Array<double> sliceY(trainY.begin(), trainY.begin() + DataList.back().second);
	Eigen::MatrixXd points(sliceX.size(), 2);

	for (int i = 0; i < sliceX.size(); i++)
	{
		points(i, 0) = sliceX[i];
		points(i, 1) = sliceY[i];
	}

	func3->ConstructScatter(points);
	// --- line側の描画周り end

	while (System::Update())
	{
		func->Draw();
		func3->Draw(Palette::Green);
		line->Draw(Palette::Orange);
	}
}
