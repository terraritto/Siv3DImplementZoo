#pragma once
#include <Siv3D.hpp>

void NonSphericalTest()
{
	struct NonSphericalCoefficient
	{
		double k;
		double R;
		double A4;
		double A6;
		double A8;
		double A10;
		double A12;

		double CalculateZ(double y)
		{
			double y2 = y * y;
			double y4 = y2 * y2;
			double r2 = R * R;
			double a1 = y2 / (R * (1.0 + Sqrt(1.0 - (1.0 + k) * y2 / r2)));
			double a4 = A4 * y4;
			double a6 = A6 * y4 * y2;
			double a8 = A8 * y4 * y4;
			double a10 = A10 * y4 * y4 * y2;
			double a12 = A12 * y4 * y4 * y4;

			return a1 + a4 + a6 + a8 + a10 + a12;
		}
	};

	NonSphericalCoefficient GeltecSampleObject; // 物側
	NonSphericalCoefficient GeltecSampleImage; // 像側

	{
		GeltecSampleObject.R = 5.09239;
		GeltecSampleObject.k = -0.47317;
		GeltecSampleObject.A4 = 0.0;
		GeltecSampleObject.A6 = -2.257 * Pow(10.0, -6);
		GeltecSampleObject.A8 =  2.412 * Pow(10.0, -9);
		GeltecSampleObject.A10 = -8.263 * Pow(10.0, -9);
		GeltecSampleObject.A12 = 0.0;
	}

	{
		GeltecSampleImage.R = -56.20310;
		GeltecSampleImage.k = 0.0;
		GeltecSampleImage.A4 = 5.124 * Pow(10.0, -4);
		GeltecSampleImage.A6 = -2.266 * Pow(10.0, -5);
		GeltecSampleImage.A8 = 3.33 * Pow(10.0, -7);
		GeltecSampleImage.A10 = 0.0;
		GeltecSampleImage.A12 = 0.0;
	}

	Array<std::pair<double, double>> objectSampleList;
	Array<std::pair<double, double>> imageSampleList;

	// 計算
	double step = 8.0 / 40.0;
	for (double y = -4.0; y <= 4.0 + step / 2.0; y += step)
	{
		objectSampleList.push_back({ y, GeltecSampleObject.CalculateZ(y)});
		imageSampleList.push_back({ y, GeltecSampleImage.CalculateZ(y)});
	}

	// Grid(単位はmm)
	constexpr double GRID_X_MAX = 20.0;
	constexpr double GRID_X_MIN = 0.0;
	constexpr double GRID_Y_MAX = 8.0;
	constexpr double GRID_Y_MIN = -8.0;

	constexpr double GRID_PER_UNIT_X = 2.0;
	constexpr double GRID_PER_UNIT_Y = 2.0;


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
		// Y軸平行のgrid線
		for (double start = GRID_X_MIN; start <= GRID_X_MAX; start += GRID_PER_UNIT_X)
		{
			double x = sceneSize.x * (start - GRID_X_MIN) / (GRID_X_MAX - GRID_X_MIN);
			Line{ Vec2{x, 0.0}, Vec2{x, sceneSize.y} }.draw(Palette::White);
		}

		// X軸平行のgrid線
		for (double start = GRID_Y_MIN; start <= GRID_Y_MAX; start += GRID_PER_UNIT_Y)
		{
			double y = sceneSize.y * (start - GRID_Y_MIN) / (GRID_Y_MAX - GRID_Y_MIN);
			Line{ Vec2{0.0, y}, Vec2{sceneSize.x, y} }.draw(Palette::White);
		}

		// 物側
		for (int i = 0; i < objectSampleList.size() - 1; i++)
		{
			auto prev = Vec2{ ConvertX(objectSampleList[i].second + GRID_X_MAX / 2.0), ConvertY(objectSampleList[i].first) };
			auto next = Vec2{ ConvertX(objectSampleList[i+1].second + GRID_X_MAX / 2.0), ConvertY(objectSampleList[i+1].first) };

			Line{ prev, next }.draw(3, Palette::Red);
		}

		// 像側
		for (int i = 0; i < imageSampleList.size() - 1; i++)
		{
			auto prev = Vec2{ ConvertX(imageSampleList[i].second + GRID_X_MAX / 2.0 + 5), ConvertY(imageSampleList[i].first) };
			auto next = Vec2{ ConvertX(imageSampleList[i + 1].second + GRID_X_MAX / 2.0 + 5), ConvertY(imageSampleList[i + 1].first) };

			Line{ prev, next }.draw(3, Palette::Red);
		}

		// Lensを結ぶ線の描画
		auto prevBottom = Vec2{ ConvertX(objectSampleList[0].second + GRID_X_MAX / 2.0), ConvertY(objectSampleList[0].first) };
		auto nextBottom = Vec2{ ConvertX(imageSampleList[0].second + GRID_X_MAX / 2.0 + 5), ConvertY(imageSampleList[0].first) };
		auto prevTop = Vec2{ ConvertX(objectSampleList.back().second + GRID_X_MAX / 2.0), ConvertY(objectSampleList.back().first) };
		auto nextTop = Vec2{ ConvertX(imageSampleList.back().second + GRID_X_MAX / 2.0 + 5), ConvertY(imageSampleList.back().first) };
		Line{ prevBottom, nextBottom }.draw(3, Palette::Red);
		Line{ prevTop, nextTop }.draw(3, Palette::Red);
	}
}
