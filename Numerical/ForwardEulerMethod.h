#pragma once
#include "Siv3D.hpp"

void ForwardEulerMethodSmaple()
{
	// f = dx/dt = xを解く
	// t=0のときx=1とする
	auto TrueFunction = [](double t)
		{
			return Exp(t);
		};

	double x = 1;

	double h = 0.1;
	double dx = 0.0;

	auto ForwardEuler = [&]()
		{
			dx = x * h;
			x = x + dx;
			return x;
		};

	Array<std::tuple<double, double>> truePoints;
	Array<std::tuple<double, double>> eulerPoints;

	for (int i = 0; i < 1000; i++)
	{
		truePoints.push_back({ i / 1000.0, TrueFunction(i / 1000.0) });
	}

	for (int i = 0; h * i < 1.0; i++)
	{
		eulerPoints.push_back({ h * i , ForwardEuler() });
	}

	// 関数的に最後尾が最大値になってるはず
	double maxValue = std::get<1>(truePoints.back()) - 1.0;

	auto sceneSize = Scene::Size();
	Scene::SetBackground(Palette::White);

	while (System::Update())
	{
		for (auto& v : truePoints)
		{
			Vec2 p
			(
				std::get<0>(v) * sceneSize.x,
				sceneSize.y -  sceneSize.y * ((std::get<1>(v) - 1.0) / maxValue ) // 1.0から開始するので、その分引いておく
			);
			Circle{ p, 1.0 }.draw(Palette::Red);
		}

		for (auto& v : eulerPoints)
		{
			Vec2 p
			(
				std::get<0>(v) * sceneSize.x,
				sceneSize.y - sceneSize.y * ((std::get<1>(v) - 1.0) / maxValue)
			);
			Circle{ p, 5.0 }.draw(Palette::Blue);
		}
	}
}
