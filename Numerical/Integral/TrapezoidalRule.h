#pragma once
#include <Siv3D.hpp>

void trapezoidalRuleSample()
{
	auto f = [](double x)
	{
		return Sin(x) * Sin(x);
	};

	Scene::SetBackground(Palette::Black);

	constexpr double a = 0.0;
	constexpr double b = Math::Pi;

	double result = 0.0;
	int n = 1000;
	double h = (b - a) / n;

	auto Calculate = [&]()
		{
			result = 0.0;
			h = (b - a) / n;

			for (int i = 0; i <= n; i++)
			{
				if (i == 0 || i == n)
				{
					result += f(a + i * h) / 2.0;
				}
				else
				{
					result += f(a + i * h);
				}
			}

			result = result * h;
		};

	Calculate();

	while (System::Update())
	{
		ClearPrint();

		Print << U"TrapezoidalRule: " << U"{:.20f}"_fmt(result);
		Print << U"Reference: " << U"{:.20f}"_fmt(Math::Pi / 2.0);
		Print << U"n=" << Format(n);

		bool isUpdate = false;
		if (Key1.down())
		{
			n -= 100; isUpdate = true;
		}
		else if (Key2.down())
		{
			n += 100; isUpdate = true;
		}

		if (isUpdate) { Calculate(); }
	}
}
