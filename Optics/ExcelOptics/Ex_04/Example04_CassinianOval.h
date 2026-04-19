#pragma once
#include <Siv3D.hpp>

void CassinianOvalTest()
{
	struct OvalCoefficient
	{
		double a = 0.5;
		double b = 1.0;

		bool IsEqual(Vec2 point)
		{
			double x = point.x;
			double y = point.y;
			double xx = x * x;
			double yy = y * y;
			double aa = a * a;
			double aaaa = aa * aa;
			double bb = b * b;
			double bbbb = bb * bb;

			double xAddy = xx + yy;
			double xSuby = xx - yy;

			double l1 = xAddy * xAddy;
			double l2 = 2.0 * bb * xSuby;
			double l3 = aaaa - bbbb;
			double l = l1 - l2 - l3;

			return Abs(l) < 1.0e-2;
		}
	};

	OvalCoefficient coefficient;
	Array<std::pair<double, double>> sampleList;

	// 座標系の定義
	constexpr double GRID_X_MAX = 2.0;
	constexpr double GRID_X_MIN = -2.0;
	constexpr double GRID_Y_MAX = 2.0;
	constexpr double GRID_Y_MIN = -2.0;

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

		isChange |= SimpleGUI::Slider(U"a: ", coefficient.a, -2.0, 2.0, { 10, 10 });
		isChange |= SimpleGUI::Slider(U"b: ", coefficient.b, -2.0, 2.0, { 10, 50 });

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
