#pragma once
#include "../PRML.h"

void GaussianSample2()
{
	Scene::SetBackground(Palette::White);

	std::shared_ptr<SimpleGrid> grid = std::make_shared<SimpleGrid>();

	grid->SetXGridRegion(Vec2(-1.0, 1.0));
	grid->SetYGridRegion(Vec2(0.0, 10.0));
	grid->SetGridStep(Vec2(1.0, 1.0));

	std::shared_ptr<GaussianPdfFunction> gaussian = std::make_shared<GaussianPdfFunction>(grid, ConvertMatrixFromNumeric<double>(0.0), ConvertMatrixFromNumeric<double>(0.1));
	std::shared_ptr<GaussianPdfFunction> model = std::make_shared<GaussianPdfFunction>(grid, gaussian, ConvertMatrixFromNumeric<double>(0.1));
	Array<std::shared_ptr<FunctionBase>> functionList;

	int size = grid->GetWidth();
	Eigen::MatrixXd meshData = MakeGrid1D(size, -1.0f, 1.0f);

	// その1(N=0)
	Eigen::MatrixXd result;
	{
		if (auto pMu = model->GetMu().lock())
		{
			result = pMu->Pdf(meshData);
		}

		std::shared_ptr<FunctionBase> func = std::make_shared<FunctionBase>(grid);
		func->CustomConstructLine(ConvertArrayFromMatrix(result));
		functionList.emplace_back(func);
	}

	// その2(N=1)
	Eigen::MatrixXd data = MakeNormalDistributionAxis(1, 0.8, 0.1, 1);
	{
		model->Fit(data);
		if (auto pMu = model->GetMu().lock())
		{
			result = pMu->Pdf(meshData);
		}

		std::shared_ptr<FunctionBase> func = std::make_shared<FunctionBase>(grid);
		func->CustomConstructLine(ConvertArrayFromMatrix(result));
		functionList.emplace_back(func);
	}

	// その3(N=2)
	{
		data = MakeNormalDistributionAxis(1, 0.8, 0.1, 1);
		model->Fit(data);
		if (auto pMu = model->GetMu().lock())
		{
			result = pMu->Pdf(meshData);
		}

		std::shared_ptr<FunctionBase> func = std::make_shared<FunctionBase>(grid);
		func->CustomConstructLine(ConvertArrayFromMatrix(result));
		functionList.emplace_back(func);
	}

	// その4(N=10)
	{
		data = MakeNormalDistributionAxis(8, 0.8, 0.1, 1);
		model->Fit(data);
		if (auto pMu = model->GetMu().lock())
		{
			result = pMu->Pdf(meshData);
		}

		std::shared_ptr<FunctionBase> func = std::make_shared<FunctionBase>(grid);
		func->CustomConstructLine(ConvertArrayFromMatrix(result));
		functionList.emplace_back(func);
	}

	Array<Color> colorList{Palette::Green, Palette::Chocolate, Palette::Bisque, Palette::Lightskyblue };

	while (System::Update())
	{
		grid->Draw();

		for (int i = 0; i < functionList.size(); i++)
		{
			functionList[i]->Draw(colorList[i]);
		}
	}
}
