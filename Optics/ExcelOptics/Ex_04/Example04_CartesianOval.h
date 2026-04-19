#pragma once
#include <Siv3D.hpp>

void CartesianOvalTest()
{
	struct OvalCoefficient
	{
		double m = 1;
		double a = 0.5;
		double c = 1.0;


		bool IsEqual(Vec2 point)
		{
			double x = point.x;
			double y = point.y;
			double xx = x * x;
			double yy = y * y;
			double mm = m * m;
			double aa = a * a;
			double cc = c * c;
			double xy2 = xx + yy;

			// left
			double l1 = (1.0 - mm) * xy2;
			double l2 = 2 * mm * c * x;
			double l3 = aa;
			double l4 = mm * cc;
			double l = l1 + l2 + l3 - l4;
			l = l * l;

			// right
			double r = 4 * aa * xy2;
			
			return Abs(l - r) < 1.0e-2;
		}
	};

	OvalCoefficient coefficient;
	Array<std::pair<double, double>> sampleList;

	// 座標系の定義
	constexpr double GRID_X_MAX = 4.0;
	constexpr double GRID_X_MIN = -4.0;
	constexpr double GRID_Y_MAX = 4.0;
	constexpr double GRID_Y_MIN = -4.0;

	// 計算
	auto Calculate = [&]()
		{
			sampleList.clear();

			double step = 1.0 / 200.0;
			for (double y = GRID_Y_MIN; y <= GRID_Y_MAX + step / 2.0; y += step)
			{
				for (double x = GRID_X_MIN; x <= GRID_X_MAX + step / 2.0; x += step)
				{
					if (coefficient.IsEqual({ x,y }))
					{
						sampleList.push_back({ x,y });
					}
				}
			}
		};
	Calculate();

	auto sceneSize = Scene::Size();

	auto ConvertX = [&](double x)
		{
			return sceneSize.x * (x - GRID_X_MIN) / (GRID_X_MAX - GRID_X_MIN);
		};

	auto ConvertY = [&](double y)
		{
			return sceneSize.y * (y - GRID_Y_MIN) / (GRID_Y_MAX - GRID_Y_MIN);
		};

	while (System::Update())
	{
		bool isChange = false;

		isChange |= SimpleGUI::Slider(U"a: ", coefficient.a, -2.0, 2.0, {10, 10});
		isChange |= SimpleGUI::Slider(U"m: ", coefficient.m, -2.0, 2.0, {10, 50});
		isChange |= SimpleGUI::Slider(U"c: ", coefficient.c, -2.0, 2.0, {10, 100});

		if (isChange)
		{
			Calculate();
		}

		// 物側
		for (int i = 0; i < sampleList.size(); i++)
		{
			auto point = sampleList[i];
			Circle{ ConvertX(point.first), ConvertY(point.second), 2 }.draw(Palette::Red);
		}
	}
}
