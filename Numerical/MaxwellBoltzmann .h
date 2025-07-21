#pragma once
#include <Siv3D.hpp>

void MaxwellBoltzmann()
{
	// ボルツマン-マクスウェル分布を図示
	double distMin = -0.0001, distMax = 0.004;
	double velocityMin = 0, velocityMax = 10000;
	double m = 1.67 * Pow(10, -27); // 中性子質量
	double k = 1.38 * Pow(10, -23); // ボルツマン定数

	// 分布の計算
	auto Distribution = [&](double v, double T)
		{
			auto a = 4 * Math::Pi * v * v;
			auto b = Pow(m / (2.0 * k * T), 1.5);
			auto c = Exp(-(m * v * v) / (2.0 * k * T));
			return a * b * c;
		};

	auto offsetT = 100.0;
	auto steps = 1000;

	Array<std::pair<int, Array<double>>> velocityList;

	for (int i = 0; i < 11; i++)
	{
		std::pair<int, Array<double>> velocityData;
		velocityData.first = i;
		for (int j = 0; j < steps; j++)
		{
			auto vv = Math::Lerp(velocityMin, velocityMax, j / static_cast<double>(steps));
			auto tt = i * offsetT;
			velocityData.second.push_back(Distribution(vv, tt));
		}
		velocityList.push_back(velocityData);
	}

	auto sceneSize = Scene::Size();
	Scene::SetBackground(Palette::White);

	while (System::Update())
	{
		// 点として描画
		for (auto& velocityData : velocityList)
		{
			auto color = Colormap01(velocityData.first / 11.0);

			for (int i = 0; i < velocityData.second.size(); i++)
			{
				auto vv = Math::Lerp(velocityMin, velocityMax, i / static_cast<double>(steps));
				double vR = 1.0 - (velocityMax - vv) / (velocityMax - velocityMin);
				vR *= sceneSize.x;
				double distR = (distMax - velocityData.second[i]) / (distMax - distMin);
				distR = distR * sceneSize.y;

				Circle{ {vR, distR},2.0 }.draw(color);
			}
		}
	}
}
