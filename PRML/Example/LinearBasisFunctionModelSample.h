#pragma once
#include "../PRML.h"

void LinearBasisFunctionModelSample()
{
	Scene::SetBackground(Palette::White);
	constexpr size_t Num = 11;

	std::shared_ptr<SimpleGrid> grid = std::make_shared<SimpleGrid>();
	grid->SetYGridRegion(Vec2(0.0f, 3.0f));
	// std::unique_ptr<PolynomialFeature> poly = std::make_unique<PolynomialFeature>(Num);
	// std::unique_ptr<GaussianFeature> poly = std::make_unique<GaussianFeature>(MakeGrid1D(11, -1.0, 1.0), 0.1);
	std::unique_ptr<SigmoidalFeature> poly = std::make_unique<SigmoidalFeature>(MakeGrid1D(11, -1.0, 1.0), 10.0);

	Array<std::shared_ptr<FunctionBase>> featureList;
	Array<Color> colorList
	{
		Palette::Green, Palette::Chocolate, Palette::Bisque, Palette::Black,
		Palette::Darkcyan, Palette::Darksalmon, Palette::Coral, Palette::Aquamarine,
		Palette::Darkviolet, Palette::Darkkhaki, Palette::Moccasin, Palette::Darkseagreen
	};

	//Array<double> meshData = ConvertArrayFromMatrix(MakeGrid1D(grid->GetWidth(), -1.0, 1.0f));
	Eigen::MatrixXd meshData = MakeGrid1D(grid->GetWidth(), -1.0, 1.0f);

	auto data = poly->Transform<double>(meshData);

	for (size_t i = 0; i < Num + 1; i++)
	{
		Array<double> extractData;

		for (auto& point : data)
		{
			extractData.push_back(point[i]);
		}

		// グラフを作成
		std::shared_ptr<FunctionBase> featureFunc = std::make_shared<FunctionBase>(grid);
		featureFunc->CustomConstructLine(extractData);
		featureList.emplace_back(featureFunc);
	}

	while (System::Update())
	{
		grid->Draw();

		for (size_t i = 0; i < featureList.size(); i++)
		{
			featureList[i]->Draw(colorList[i]);
		}
	}
}
