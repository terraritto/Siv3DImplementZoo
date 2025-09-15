#pragma once
#include <Siv3D.hpp>

void CohenSutherlandSample()
{
	Scene::SetBackground(Palette::Black);

	constexpr double WINDOW_X_MIN = 200.0;
	constexpr double WINDOW_X_MAX = 600.0;
	constexpr double WINDOW_Y_MIN = 200.0;
	constexpr double WINDOW_Y_MAX = 400.0;

	constexpr int INSIDE	= 0;	// Windowの内側:0000

	auto ComputeRegion = [&](Vec2 p)
		{
			int region = INSIDE;

			if (p.x < WINDOW_X_MIN)
			{
				region |= 1;	 	// Windowの左側:0001
			}
			else if (WINDOW_X_MAX < p.x)
			{
				region |= 1 << 1;	// Windowの右側:0010
			}

			if (p.y < WINDOW_Y_MIN)
			{
				region |= 1 << 2;	// WINDOWの上側:0100
			}
			else if (WINDOW_Y_MAX < p.y)
			{
				region |= 1 << 3;	// WINDOWの下側:1000
			}

			return region;
		};

	auto ComputeCohenSutherland = [&](Vec2 p1, Vec2 p2, std::pair<Vec2, Vec2>& result)
		{
			int regionP1 = ComputeRegion(p1);
			int regionP2 = ComputeRegion(p2);

			while (true)
			{
				if (regionP1 == 0 && regionP2 == 0)
				{
					// 両方ともWindow内
					result.first = p1; result.second = p2;
					return true;
				}
				else if (regionP1 & regionP2)
				{
					// 両方ともWindow外の同じ位置
					return false;
				}
				else
				{
					// 両方が全く違う位置の場合
					// 少なくともどっちかは外なので、それをpickする
					int outRegion = regionP1 != INSIDE ? regionP1 : regionP2;

					Point p;
					if (outRegion & 1)
					{
						// LEFT
						p.y = p1.y + (p2.y - p1.y) * (WINDOW_X_MIN - p1.x) / (p2.x - p1.x);
						p.x = WINDOW_X_MIN;
					}
					else if (outRegion & (1 << 1))
					{
						// RIGHT
						p.y = p1.y + (p2.y - p1.y) * (WINDOW_X_MAX - p1.x) / (p2.x - p1.x);
						p.x = WINDOW_X_MAX;
					}
					else if (outRegion & (1 << 2))
					{
						// TOP
						p.x = p1.x + (p2.x - p1.x) * (WINDOW_Y_MIN - p1.y) / (p2.y - p1.y);
						p.y = WINDOW_Y_MIN;
					}
					else if (outRegion & (1 << 3))
					{
						// BOTTOM
						p.x = p1.x + (p2.x - p1.x) * (WINDOW_Y_MAX - p1.y) / (p2.y - p1.y);
						p.y = WINDOW_Y_MAX;
					}

					// 新しい範囲を指定
					bool isRegionP1 = outRegion == regionP1;
					Vec2& newP = isRegionP1 ? p1 : p2;
					int& newRegion = isRegionP1 ? regionP1 : regionP2;
					newP = p;
					newRegion = ComputeRegion(newP);
				}
			}
		};

	constexpr int LINE_NUM = 100;
	const Size sceneSize = Scene::Size();
	Array<Line> beforeLines, afterLinesClip, afterLineRemove;
	for (int i = 0; i < LINE_NUM; i++)
	{
		Vec2 begin = { Random(0, sceneSize.x - 1), Random(0, sceneSize.y - 1) };
		Vec2 end = { Random(0, sceneSize.x - 1), Random(0, sceneSize.y - 1) };
		beforeLines.push_back(Line{ begin, end });
	}

	while (System::Update())
	{
		// Rectangleの描画
		RectF{ WINDOW_X_MIN, WINDOW_Y_MIN, WINDOW_X_MAX - WINDOW_X_MIN, WINDOW_Y_MAX - WINDOW_Y_MIN }.drawFrame(1.0, Palette::Blue);

		if (KeyEnter.pressed())
		{
			if (beforeLines.size() > 0)
			{
				Line line = beforeLines.back(); beforeLines.pop_back();
				std::pair<Vec2, Vec2> result;

				if (ComputeCohenSutherland(line.begin, line.end, result))
				{
					line.begin = result.first; line.end = result.second;
					afterLinesClip.push_back(line);
				}
				else
				{
					afterLineRemove.push_back(line);
				}
			}
		}

		if (KeySpace.down())
		{
			beforeLines.clear(); afterLinesClip.clear(); afterLineRemove.clear();
			for (int i = 0; i < LINE_NUM; i++)
			{
				Vec2 begin = { Random(0, sceneSize.x - 1), Random(0, sceneSize.y - 1) };
				Vec2 end = { Random(0, sceneSize.x - 1), Random(0, sceneSize.y - 1) };
				beforeLines.push_back(Line{ begin, end });
			}
		}

		for (const auto& line : beforeLines)
		{
			line.draw(Palette::Green);
		}

		for (const auto& line : afterLinesClip)
		{
			line.draw(Palette::Red);
		}

		for (const auto& line : afterLineRemove)
		{
			line.draw(Palette::Yellow);
		}
	}
}
