#pragma once
#include <Siv3D.hpp>

void SimpsonsRuleSample()
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
	double h = (b - a) / (2.0 * n);

	auto Calculate = [&]()
		{
			result = 0.0;
			h = (b - a) / (2.0 * n);

			for (int i = 0; i <= 2 * n; i++)
			{
				double next = a + i * h;
				double factor = IsOdd(i) ? 2.0 : 4.0;

				if (i == 0 || i == 2 * n)
				{
					factor = 1.0;
				}

				result += factor * f(next);
			}

			result = result * h / 3.0;
		};

	Calculate();

	while (System::Update())
	{
		ClearPrint();

		Print << U"SimpsonRule: " << U"{:.20f}"_fmt(result);
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
