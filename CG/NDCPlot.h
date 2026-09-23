#pragma once
#include <Siv3D.hpp>

void NDCPlot()
{
	constexpr double NEAR_PLANE= 1.0;
	constexpr double FAR_PLANE = 5.0;

	constexpr double Z_MIN = -6.0;
	constexpr double Z_MAX = 0.5;
	constexpr double Z_NDC_MIN = -1.2;
	constexpr double Z_NDC_MAX = 1.2;

	auto CalculateNDC = [&](double z)
		{
			const double A = (FAR_PLANE + NEAR_PLANE) / (FAR_PLANE - NEAR_PLANE);
			const double B = 2.0 * FAR_PLANE * NEAR_PLANE / (FAR_PLANE - NEAR_PLANE);
			return (A * z + B) / Abs(z);
		};

	Scene::SetBackground(Palette::White);
	auto sceneSize = Scene::Size();

	while (System::Update())
	{
		// z_ndc=-1,-5
		{
			{
				double x_prev = 0.0, x_next = sceneSize.x;
				double zNdc_1 = CalculateNDC(-1.0);
				double zNdc_2 = CalculateNDC(-5.0);

				double y1 = (1.0 - (zNdc_1 - Z_NDC_MIN) / (Z_NDC_MAX - Z_NDC_MIN)) * sceneSize.y;
				double y2 = (1.0 - (zNdc_2 - Z_NDC_MIN) / (Z_NDC_MAX - Z_NDC_MIN)) * sceneSize.y;

				Line{ Vec2{x_prev,y1}, Vec2{x_next,y1} }.draw(Palette::Green);
				Line{ Vec2{x_prev,y2}, Vec2{x_next,y2} }.draw(Palette::Green);
			}
		}

		// z=-1,z=-5
		{
			{
				double z1 = -1.0, z2 = -5.0;
				double x1 = (z1 - Z_MIN) / (Z_MAX - Z_MIN) * sceneSize.x;
				double x2 = (z2 - Z_MIN) / (Z_MAX - Z_MIN) * sceneSize.x;


				Line{ Vec2{x1,0}, Vec2{x1,sceneSize.y} }.draw(Palette::Blue);
				Line{ Vec2{x2,0}, Vec2{x2,sceneSize.y} }.draw(Palette::Blue);
			}
		}

		for (int i = 0; i < 100; i++)
		{
			double zPrev = Math::Lerp(Z_MIN, -0.05, i / static_cast<double>(100));
			double zNext = Math::Lerp(Z_MIN, -0.05, (i+1) / static_cast<double>(100));
			double zNdcPrev = CalculateNDC(zPrev);
			double zNdcNext = CalculateNDC(zNext);

			double x_prev = (zPrev - Z_MIN) / (Z_MAX - Z_MIN) * sceneSize.x;
			double x_next = (zNext - Z_MIN) / (Z_MAX - Z_MIN) * sceneSize.x;
			double y_prev = ((zNdcPrev - Z_NDC_MIN) / (Z_NDC_MAX - Z_NDC_MIN)) * sceneSize.y;
			double y_next = ((zNdcNext - Z_NDC_MIN) / (Z_NDC_MAX - Z_NDC_MIN)) * sceneSize.y;

			Line{ Vec2{x_prev,y_prev}, Vec2{x_next,y_next} }.draw(Palette::Red);
		}
	}
}
