#pragma once
#include <Siv3D.hpp>

void RectStretchedTest()
{
	Scene::SetBackground(ColorF{ 0.6, 0.8, 0.7 });

	Rect original{ 100,100,200,200 };
	Rect stretched = original.stretched(Arg::bottom = 50);

	while (System::Update())
	{
		original.draw();
		stretched.movedBy(300, 0).draw();
	}
}
