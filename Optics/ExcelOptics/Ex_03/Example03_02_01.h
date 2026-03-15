#pragma once
#include <Siv3D.hpp>

void Example03_02_01()
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
	constexpr double GRID_X_MAX = 250.0;
	constexpr double GRID_X_MIN = 0.0;
	constexpr double GRID_Y_MAX = 20.0;
	constexpr double GRID_Y_MIN = -20.0;

	constexpr double GRID_PER_UNIT_X = 50.0;
	constexpr double GRID_PER_UNIT_Y = 5.0;

	// レンズ条件
	double f1 = 100.0;						// f1 = 100mm
	double d1 = 50.0;						// d1 = 50mm
	double d2 = 100.0;						// d2 = 100mm
	double d3 = 100.0;						// d3 = 100mm
	double na = 0.55;						// NA = 0.55

	// 高さを列挙
	Array<double> w1List = { -5,-2.5,0,2.5,5 };
	Array<Color> colorList = { Palette::Yellow, Palette::Red, Palette::Green, Palette::Azure, Palette::Orange };

	// 角度を列挙
	Array<double> thetaList = { -0.1, 0, 0.1 };

	// 計算
	std::map<int, Array<double>> w2Map, theta2Map, w3Map;

	for (int i = 0; i < w1List.size(); i++)
	{
		Array<double> w2List, theta2List, w3List;

		for (auto theta : thetaList)
		{
			w2List.push_back(CalculateObjectHeight(w1List[i], d2, theta));
			theta2List.push_back(CalculateTheta(theta, w2List.back(), f1));
			w3List.push_back(CalculateObjectHeight(w2List.back(), d3, theta2List.back()));
		}

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

		for (int j = 0; j < w1List.size(); ++j)
		{
			Color color = colorList[j];
;
			auto w2List = w2Map[j];
			auto w3List = w3Map[j];

			for (int i = 0; i < listSize; i++)
			{
				// w0からw1へ
				Vec2 w0Pos = { ConvertX(0), ConvertY(w1List[j])};
				Vec2 w1Pos = { ConvertX(d1), ConvertY(w1List[j]) };
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
			// ガルバノミラー
			s3d::Line{ Vec2{ConvertX(d1), ConvertY(-12)}, Vec2{ConvertX(d1), ConvertY(12)}}.draw(3.0, Palette::Red);

			Vec2 center = { ConvertX(d1 + d2), ConvertY(0) };
			s3d::Ellipse{ center, 30, 250 }.drawFrame(1.0, Palette::Pink);
		}
	}
}
