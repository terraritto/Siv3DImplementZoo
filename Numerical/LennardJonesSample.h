#pragma once
#include "Siv3D.hpp"

void LennardJonesSmaple()
{
	// potentialを計算(Lenard Jonesポテンシャル)
	double rMin = 0.8, rMax = 2.0;
	double potentialMin = -2.0, potentialMax = 10.0;
	auto Pontential = [](double r)
		{ return 4.0 * (Pow(1/r, 12) - Pow(1/r, 6)); };

	auto sceneSize = Scene::Size();
	Scene::SetBackground(Palette::White);

	int steps = 1000;
	Array<std::pair<double, double>> potentials(steps);

	const Font font{ FontMethod::MSDF, 24 };

	for (int i = 0; i < steps; i++)
	{
		double r = Math::Lerp(rMin, rMax, i / static_cast<double>(steps));
		double potential = Pontential(r);
		potentials[i] = { r, potential };
	}

	while (System::Update())
	{
		// 点として描画
		for (auto& potential : potentials)
		{
			double pointR = 1.0 - (rMax - potential.first) / (rMax - rMin);
			pointR *= sceneSize.x;
			double pointPotential = (potentialMax - potential.second) / (potentialMax - potentialMin);
			if (pointPotential > 1.0 || pointPotential < 0.0) { continue; }
			pointPotential = pointPotential * sceneSize.y;

			Circle{ {pointR, pointPotential},2.0 }.draw(Palette::Red);
		}

		// Line
		double zeroPotential = potentialMax / (potentialMax - potentialMin);
		zeroPotential = zeroPotential * sceneSize.y;
		Line({ 0, zeroPotential }, { sceneSize.x, zeroPotential }).draw(2.0, Palette::Green);
		
		font(U"↑ U*(r*)").drawAt({ 120,20 }, Palette::Blue);
		
		font(U"→ r*").drawAt({ sceneSize.xy() - Vec2{30, 20} }, Palette::Blue);
	}
}
