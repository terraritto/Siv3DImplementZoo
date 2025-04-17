#pragma once
#include <Siv3D.hpp>

void SmallestEnclosingCircleTest()
{
	Scene::SetBackground(ColorF{ 0.6, 0.8, 0.7 });
	const Font font{ FontMethod::MSDF, 48, Typeface::Bold };

	Array<Vec2> prevPoints;

	while (System::Update())
	{
		if (SimpleGUI::Button(U"Red", Vec2{ 100, 100 }))
		{
			prevPoints.clear();
			for (int i = 0; i < 20; i++)
			{
				prevPoints.push_back({ Vec2{Random(), Random()} * 200 + Vec2{300, 200}});
			}
		}

		for (auto& point : prevPoints)
		{
			Circle{ point, 5.0 }.draw();
		}

		if (prevPoints.size() > 0)
		{
			Geometry2D::SmallestEnclosingCircle(prevPoints).drawFrame(1.0, Palette::Red);
		}
	}
}
