#pragma once
#include <Siv3D.hpp>

void ClampAngleTest()
{
	double angleRad{};
	double minRad{};
	double maxRad{ Math::HalfPi };

	while (System::Update())
	{
		constexpr double range = Math::TwoPi * 2;
		SimpleGUI::Headline(U"angleRad: {:.4f}"_fmt(angleRad), { 20, 20 });
		SimpleGUI::Slider(angleRad, -range, range, { 200, 20 }, 280);

		SimpleGUI::Headline(U"minRad: {:.4f}"_fmt(minRad), { 20, 60 });
		SimpleGUI::Slider(minRad, -range, range, { 200, 60 }, 280);

		SimpleGUI::Headline(U"maxRad: {:.4f}"_fmt(maxRad), { 20, 100 });
		SimpleGUI::Slider(maxRad, -range, range, { 200, 100 }, 280);

		const double clampedRad = Math::ClampAngle(angleRad, minRad, maxRad);
		SimpleGUI::Headline(U"ClampAngle(angleRad, minRad, maxRad) -> {:.4f}"_fmt(clampedRad), { 20, 140 });

		Circle(Scene::Center(), 100)
			.drawPie(minRad, maxRad - minRad, Palette::Gray)
			.drawFrame(1.0, Palette::White);

		Line{ Scene::Center(), Circular{100, clampedRad}.toVec2() + Scene::Center() }.draw(5.0, Palette::Yellow);

		Line{ Scene::Center(), Circular{100, angleRad}.toVec2() + Scene::Center() }.draw(2.0, Palette::Red);

		Line{ Scene::Center(), Circular{100, minRad}.toVec2() + Scene::Center() }.draw(2.0, Palette::Green);

		Line{ Scene::Center(), Circular{100, maxRad}.toVec2() + Scene::Center() }.draw(2.0, Palette::Blue);
	}
}
