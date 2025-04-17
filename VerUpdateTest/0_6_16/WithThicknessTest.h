#pragma once
#include <Siv3D.hpp>

void WithThicknessTest()
{
	Scene::SetBackground(ColorF{ 0.6, 0.8, 0.7 });

	while (System::Update())
	{
		Line{ 100, 100, 500, 100 }.draw();
		Line{ 100, 400, 500, 400 }.withThickness(50).draw();
	}
}
