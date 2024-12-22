#pragma once
#include "../PRML.h"

void GaussianSample()
{
	Scene::SetBackground(Palette::White);

	std::shared_ptr<SimpleGrid> grid = std::make_shared<SimpleGrid>();

	grid->SetXGridRegion(Vec2(-8.0, 8.0));
	grid->SetYGridRegion(Vec2(-8.0, 8.0));
	grid->SetGridStep(Vec2(1.0, 1.0));

	std::shared_ptr<MultiVariateGaussianPdfFunction> gaussian = std::make_shared<MultiVariateGaussianPdfFunction>(grid);
	std::shared_ptr<FunctionBase> func = std::make_shared<FunctionBase>(grid);
	std::shared_ptr<FunctionBase> func2 = std::make_shared<FunctionBase>(grid);

	int size = 100;
	Eigen::MatrixXd data = MakeNormalDistributionAxis(size);
	Eigen::MatrixXd MeshGridData = MakeSquareGrid(size, -10.0f, 10.0f);

	// 計算
	gaussian->Fit(data);
	Eigen::MatrixXd Result = gaussian->Pdf(MeshGridData);

	// 表示
	Eigen::MatrixXd mu = gaussian->GetMu();
	Eigen::MatrixXd cov = gaussian->GetCov();

	Print << U"mu:[" << mu(0,0) << U", " << mu(1,0) << U"]";
	Print << U"cov:[[" << cov(0,0) << U", " << cov(0,1) << U"],";
	Print << U"[" << cov(1,0) << U", " << cov(1,1) << U"]]";

	// XY,Z->XYZ->抽出の工程
	Eigen::MatrixXd MeshGridResult = MakeGridXYZ(MeshGridData, Result);
	MeshGridData = ExtractDataFromZ(MeshGridResult, 0.035);

	// 描画データの構築
	func->ConstructScatter(data);
	func2->ConstructScatter(MeshGridData);

	while (System::Update())
	{
		grid->Draw();
		func->Draw(); func2->Draw();
	}
}
