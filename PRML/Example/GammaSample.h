#pragma once
#include "../PRML.h"

void GammaSample()
{
	Scene::SetBackground(Palette::White);

	std::shared_ptr<SimpleGrid> grid = std::make_shared<SimpleGrid>();

	grid->SetXGridRegion(Vec2(0.0f, 2.0f));
	grid->SetYGridRegion(Vec2(0.0f, 2.0f));
	grid->SetGridStep(Vec2(0.1f, 0.1f));

	Array<std::unique_ptr<FunctionBase>> funcList;
	const Array<Vec2> ParameterList = { {0.1,0.1}, {1,1}, {2,3},{4,6} };
	Array<Color> colorList{Palette::Green, Palette::Chocolate, Palette::Bisque, Palette::Lightskyblue};

	for (int i = 0; i < ParameterList.size(); i++)
	{
		const Vec2 param = ParameterList[i];
		std::unique_ptr<FunctionBase> func = std::make_unique<GammaPdfFunction>(grid, param.x, param.y);
		func->ConstructLine();
		funcList.emplace_back(std::move(func));
	}

	while (System::Update())
	{
		grid->Draw();

		for (int i = 0; i < colorList.size(); i++)
		{
			funcList[i]->Draw(colorList[i]);
		}
	}
}

