#pragma once
#include <Siv3D.hpp>

void Example03_01_02()
{
	auto CalculateObjectHeight = [](double w, double d, double theta)
		{
			return w + d * theta;
		};

	auto CalculateTheta = [](double theta, double w, double f)
		{
			return theta - w / f;
		};

	auto sceneSize = Scene::Size();

	// Grid(単位はmm)
	constexpr double GRID_X_MAX = 450.0;
	constexpr double GRID_X_MIN = 0.0;
	constexpr double GRID_Y_MAX = 8.0;
	constexpr double GRID_Y_MIN = -8.0;

	constexpr double GRID_PER_UNIT_X = 50.0;
	constexpr double GRID_PER_UNIT_Y = 2.0;

	// レンズ条件
	double f1 = 4.0;						// f1 = 4.0mm
	double f2 = 200.0;						// f2 = 200.0mm
	double d1 = 4.0;						// d1 = 4.0mm
	double d2 = 204.0;						// d2 = 204mm
	double d3 = 200.0;						// d3 = 200mm
	double na = 0.55;						// NA = 0.55
	double maxNa = 2.2;						// a = 最大レンズ開口 = f1 * na = 2.2mm
	double maxTheta = maxNa / d1;			// 最大角 = a/d1(rad)

	// 高さを列挙: w0 = 0, -0.1, 0.1
	Array<double> w0List = { -0.1, 0, 0.1 };
	Array<Color> colorList = { Palette::Yellow, Palette::Red, Palette::Green };

	// 角度を列挙
	Array<double> thetaList;
	const double thetaStep = maxTheta / 3;
	for (int i = -3; i <= 3; ++i)
	{
		double theta = static_cast<double>(i) * thetaStep;
		thetaList.push_back(theta);
	}

	// 計算
	std::map<int, Array<double>> w1Map, theta1Map, w2Map, theta2Map, w3Map;

	for (int i = 0; i < w0List.size(); i++)
	{
		Array<double> w1List, theta1List, w2List, theta2List, w3List;

		for (auto theta : thetaList)
		{
			w1List.push_back(CalculateObjectHeight(w0List[i], d1, theta));
			theta1List.push_back(CalculateTheta(theta, w1List.back(), f1));
			w2List.push_back(CalculateObjectHeight(w1List.back(), d2, theta1List.back()));
			theta2List.push_back(CalculateTheta(theta1List.back(), w2List.back(), f2));
			w3List.push_back(CalculateObjectHeight(w2List.back(), d3, theta2List.back()));
		}

		w1Map[i] = w1List;
		theta1Map[i] = theta1List;
		w2Map[i] = w2List;
		theta2Map[i] = theta2List;
		w3Map[i] = w3List;
	}

	int listSize = thetaList.size();

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

		for (int j = 0; j < w0List.size(); ++j)
		{
			Color color = colorList[j];
			double w0 = w0List[j];

			auto w1List = w1Map[j];
			auto w2List = w2Map[j];
			auto w3List = w3Map[j];

			for (int i = 0; i < listSize; i++)
			{
				// w0からw1へ
				Vec2 w0Pos = { ConvertX(0), ConvertY(w0) };
				Vec2 w1Pos = { ConvertX(d1), ConvertY(w1List[i]) };
				Line{ w0Pos, w1Pos }.draw(color);

				// w1からw2へ
				Vec2 w2Pos = { ConvertX(d1 + d2), ConvertY(w2List[i]) };
				Line{ w1Pos, w2Pos }.draw(color);

				// w2からw3へ
				Vec2 w3Pos = { ConvertX(d1 + d2 + d3), ConvertY(w3List[i]) };
				Line{ w2Pos, w3Pos }.draw(color);
			}
		}

		// 疑似レンズを楕円で出してみる
		{
			Vec2 center = { ConvertX(d1), ConvertY(0) };
			s3d::Ellipse{ center, 5, 100 }.drawFrame(1.0, Palette::Pink);

			center = { ConvertX(d1 + d2), ConvertY(0) };
			s3d::Ellipse{ center, 30, 300 }.drawFrame(1.0, Palette::Pink);
		}
	}
}
