#pragma once
#include "../PRML.h"

// The Evidence Approximation
void EvidenceApproximation()
{
	Scene::SetBackground(Palette::White);

	std::shared_ptr<SimpleGrid> grid = std::make_shared<SimpleGrid>();
	grid->SetXGridRegion(Vec2(-5.0, 5.0));
	grid->SetYGridRegion(Vec2(-80.0, 80.0));

	constexpr int Sample = 50 - 1;

	std::shared_ptr<CubicFunction> func = std::make_shared<CubicFunction>(grid);
	std::shared_ptr<ToyCubicFunction> func2 = std::make_shared<ToyCubicFunction>(grid, 0.0, 10);
	func->ConstructLine(); func2->ConstructCircle(Sample);

	auto trainX = func2->GetTrainX();
	auto trainY = func2->GetTrainY();

	// EvidenceとModel
	Array<double> evidenceList;
	Array<std::shared_ptr<EmpiricalBayesRegression>> regressionList;

	for (int i = 0; i < 8; i++)
	{
		std::unique_ptr<PolynomialFeature> feature = std::make_unique<PolynomialFeature>(i);
		auto trainData = feature->Transform<double>(trainX);

		auto model = std::make_shared<EmpiricalBayesRegression>(100.0, 100.0);
		model->fit(trainData, trainY);
		evidenceList.push_back(model->LogEvidence(trainData, trainY));
		regressionList.push_back(model);
	}

	// Evidenceで保証されたregressionを取り出し
	auto maxIter = std::max_element(evidenceList.begin(), evidenceList.end());
	int index = std::distance(evidenceList.begin(), maxIter);

	// 必要なデータ生成
	auto regression = regressionList[index];
	std::unique_ptr<PolynomialFeature> feature = std::make_unique<PolynomialFeature>(index);
	auto testData = feature->Transform<double>(func->GetTrainX());


	Array<double> result = regression->Predict(testData);
	std::shared_ptr<FunctionBase> func3 = std::make_shared<FunctionBase>(grid);
	func3->CustomConstructLine(result);

	std::shared_ptr<FunctionBase> func4 = std::make_shared<FunctionBase>(grid);
	grid->SetYGridRegion(Vec2(-250.0, -130.0));
	func4->ConstructLineCircle(evidenceList);

	while (System::Update())
	{
		//func->Draw();
		//func2->Draw(Palette::Blue);
		//func3->Draw(Palette::Orange);
		func4->Draw();
	}
}
