#pragma once
#include "../PRML.h"

// Parameter distribution
void ParameterDistributionSample()
{
	Scene::SetBackground(Palette::White);

	std::shared_ptr<SimpleGrid> grid = std::make_shared<SimpleGrid>();
	grid->SetXGridRegion(Vec2(-1.0, 1.0));
	grid->SetYGridRegion(Vec2(-1.0, 1.0));

	constexpr int Sample = 20 - 1;
	constexpr int LineNum = 6;

	std::shared_ptr<LinearFunction> func = std::make_shared<LinearFunction>(grid);
	std::shared_ptr<ToyLinearFunction> func2 = std::make_shared<ToyLinearFunction>(grid, 0.0, 0.1);
	func->SetAB(0.5, -0.3); func2->SetAB(0.5, -0.3);
	func->ConstructLine(); func2->ConstructCircle(Sample);

	// feature
	std::unique_ptr<PolynomialFeature> feature = std::make_unique<PolynomialFeature>(1);
	auto testData = feature->Transform<double>(func->GetTrainX());
	auto trainData = feature->Transform<double>(func2->GetTrainX());
	auto trainX = func2->GetTrainX();
	auto trainY = func2->GetTrainY();

	// 回帰
	std::unique_ptr<BayesianRegression> regression = std::make_unique<BayesianRegression>(1.0, 100);

	const Array<std::pair<int, int>> DataList = { {0,1},{1,2},{2,3} };//,{3,20} };

	// MeshGridの生成
	Eigen::MatrixXd MeshGridData = MakeSquareGrid(100, -1.0f, 1.0f);

	Eigen::MatrixXd result(10000, 1);
	Array<double> lineResult;
	// 計算
	for (int i = 0; i < DataList.size(); i++)
	{
		// fitを行う
		Array<Array<double>> sliceX(trainData.begin() + DataList[i].first, trainData.begin() + DataList[i].second);
		Array<double> sliceY(trainY.begin() + DataList[i].first, trainY.begin() + DataList[i].second);
		regression->fit(sliceX, sliceY);

		for (int j = 0; j < MeshGridData.rows(); j++)
		{
			result(j, 0) = mvn_norm_pdf(MeshGridData.row(j).transpose(), regression->GetMean(), regression->GetConv());
		}

		lineResult = regression->Predict(testData, LineNum);
	}

	// XY,Z->XYZ->抽出の工程
	Eigen::MatrixXd MeshGridResult = MakeGridXYZ(MeshGridData, result);
	MeshGridData = ExtractDataFromZ(MeshGridResult, 0.03);

	// --- scatter側の描画周り start
	//std::shared_ptr<FunctionBase> func3 = std::make_shared<FunctionBase>(grid);
	std::shared_ptr<FunctionBase> func4 = std::make_shared<FunctionBase>(grid);
	Array<std::shared_ptr<FunctionBase>> lineList;
	//func3->ConstructScatter(MeshGridData);

	// xを生成
	//Eigen::MatrixXd point(1, 2); point(0, 0) = -0.3; point(0, 1) = 0.5;
	//func4->ConstructScatter(point);
	// --- scatter側の描画周り end

	// --- line側の描画周り start
	// 線の生成
	for (int i = 0; i < LineNum; i++)
	{
		std::shared_ptr<FunctionBase> line = std::make_shared<FunctionBase>(grid);
		Array<double> sliceArray(lineResult.begin() + i * testData.size(), lineResult.begin() + (i+1) * testData.size());
		line->CustomConstructLine(sliceArray);
		lineList.push_back(line);
	}

	// scatterの用意
	Array<double> sliceX(trainX.begin(), trainX.begin() + DataList.back().second);
	Array<double> sliceY(trainY.begin(), trainY.begin() + DataList.back().second);
	Eigen::MatrixXd points(sliceX.size(), 2);

	for (int i = 0; i < sliceX.size(); i++)
	{
		points(i, 0) = sliceX[i];
		points(i, 1) = sliceY[i];
	}

	func4->ConstructScatter(points);
	// --- line側の描画周り end

	while (System::Update())
	{
		//func3->Draw(Palette::Yellow);
		func4->Draw(Palette::Green);

		for (auto& line : lineList)
		{
			line->Draw(Palette::Orange);
		}
	}
}
