#pragma once
#include "../PRML.h"

// LogisticRegressionのテスト
void LogisticRegressionSample()
{
	Scene::SetBackground(Palette::White);

	std::shared_ptr<SimpleGrid> grid = std::make_shared<SimpleGrid>();
	grid->SetXGridRegion(Vec2(-5.0, 15.0));
	grid->SetYGridRegion(Vec2(-5.0, 15.0));

	constexpr int Sample = 50 - 1;

	auto func2 = std::make_shared<ToyNormalFunction>(grid, -1.0);
	auto func3 = std::make_shared<ToyNormalFunction>(grid, 1.0);

	auto funcOutlinerX = std::make_shared<ToyNormalFunction>(grid, 5.0);
	auto funcOutlinerY = std::make_shared<ToyNormalFunction>(grid, 10.0);

	// クラス C = {0, 1}の生成
	Array<double> TestY;
	TestY.append(Array<double>(25, 0));
	TestY.append(Array<double>(30, 1));

	func2->ConstructCircle(Sample); func3->ConstructCircle(Sample);
	funcOutlinerX->ConstructCircle(5 - 1); funcOutlinerY->ConstructCircle(5 - 1);

	auto trainFirst = func2->GetTrainY();
	auto trainSecond = func3->GetTrainY();

	auto outlinerX = funcOutlinerX->GetTrainY();
	auto outlinerY = funcOutlinerY->GetTrainY();

	// 消すかも
	Eigen::MatrixXd data(25, 2);
	Eigen::MatrixXd data2(30, 2);

	for (int i = 0; i < trainFirst.size() / 2; i++)
	{
		data(i, 0) = trainFirst[i];
		data(i, 1) = trainFirst[i+25];
		data2(i, 0) = trainSecond[i];
		data2(i, 1) = trainSecond[i + 25];
	}

	// outlineも追加しておく
	for (int i = 0; i < outlinerX.size(); i++)
	{
		data2(i + 25, 0) = outlinerX[i];
		data2(i + 25, 1) = outlinerY[i];
	}
	// ここまで

	Eigen::MatrixXd train(55, 2);
	train << data, data2;

	Eigen::MatrixXd MeshGridData = MakeSquareGrid(100, -5.0f, 15.0f);

	std::unique_ptr<PolynomialFeature> feature = std::make_unique<PolynomialFeature>(1);
	auto trainData = feature->Transform(train);
	auto testData = feature->Transform(MeshGridData);

	// ここ切り替え
	auto model = std::make_shared<LeastSquaresClassifier>();
	//auto model = std::make_shared<LogisticRegression>();
	model->fit(trainData, TestY);

	auto result = model->classify(testData);

	// XY,Z->XYZ->抽出の工程
	Eigen::MatrixXd MeshGridResult = MakeGridXYZ(MeshGridData, result);
	MeshGridData = ExtractDataFromZ(MeshGridResult, 0.0, 0.5);

	std::shared_ptr<FunctionBase> func4 = std::make_shared<FunctionBase>(grid);
	std::shared_ptr<FunctionBase> func5 = std::make_shared<FunctionBase>(grid);
	std::shared_ptr<FunctionBase> func6 = std::make_shared<FunctionBase>(grid);

	func4->ConstructScatter(data);
	func5->ConstructScatter(data2);
	func6->ConstructScatter(MeshGridData);

	while (System::Update())
	{
		func6->Draw(Palette::Green);
		func4->Draw(); func5->Draw(Palette::Blue);
	}
}
