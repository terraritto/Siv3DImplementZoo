#pragma once
#include "../PRML.h"

void RMSESample()
{
	Scene::SetBackground(Palette::White);

	std::shared_ptr<SimpleGrid> grid = std::make_shared<SimpleGrid>();
	std::shared_ptr<SinFunction> func = std::make_shared<SinFunction>(grid);
	std::shared_ptr<ToySinFunction> func2 = std::make_shared<ToySinFunction>(grid, 0.0, 0.25);
	std::unique_ptr<PolynomialFeature> poly = std::make_unique<PolynomialFeature>(9);
	std::unique_ptr<LinearRegression> regression = std::make_unique<LinearRegression>();

	// Toyデータの生成
	func->ConstructLine(); func2->ConstructCircle(20);

	// RMSEを計算するだけ
	auto RMSE = [](const Array<double>& a, const Array<double>& b)
	{
		Eigen::MatrixXd aa(a.size(), 1);
		Eigen::MatrixXd bb(b.size(), 1);

		for (size_t n = 0; n < a.size(); n++)
		{
			aa(n, 0) = a[n];
			bb(n, 0) = b[n];
		}

		return Math::Sqrt((aa - bb).cwiseAbs2().mean());
	};

	constexpr size_t Num = 20;
	size_t degree = 0;

	std::random_device randomDevice;
	std::default_random_engine engine(randomDevice());
	std::normal_distribution<> normalDistribution(0.0,0.25);
	Array<double> trainErrorList, testErrorList;

	for (size_t i = 0; i < Num; i++)
	{
		poly->SetDegree(degree);

		// Polynomialの計算をしておく
		auto trainData = poly->Transform<double>(func2->GetTrainX());
		auto testData = poly->Transform<double>(func->GetTrainX());

		// 回帰
		regression->fit(trainData, func2->GetTrainY());
		Array<double> predict = regression->Predict(testData);

		// RMSEの計算
		double trainError = RMSE(regression->Predict(trainData), func2->GetTrainY());

		Array<double> testY = func->GetTrainY();
		std::for_each(testY.begin(), testY.end(), [&engine, &normalDistribution](double& value) { value += normalDistribution(engine); });
		double testError = RMSE(predict, testY);

		trainErrorList.push_back(trainError);
		testErrorList.push_back(testError);

		// degree更新
		degree += 1;
	}

	Array<std::shared_ptr<FunctionBase>> predictList;

	{
		// trainの生成
		std::shared_ptr<FunctionBase> train = std::make_shared<FunctionBase>(grid);
		train->ConstructLineCircle(trainErrorList);
		predictList.emplace_back(train);

		// testの生成
		std::shared_ptr<FunctionBase> test = std::make_shared<FunctionBase>(grid);
		test->ConstructLineCircle(testErrorList);
		predictList.emplace_back(test);
	}

	Array colorList = { Palette::Blue, Palette::Red };

	// gridの調整
	grid->SetXGridRegion({ 0.0, 10.0 });
	grid->SetYGridRegion({ 0.0, 1.0 });
	grid->SetGridStep({ 1.0, 0.1 });

	while (System::Update())
	{
		grid->Draw();

		for (size_t i = 0; i < predictList.size(); i++)
		{
			predictList[i]->Draw(colorList[i], colorList[i]);
		}
	}
}
