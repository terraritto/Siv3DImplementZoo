#pragma once
#include <Siv3D.hpp>

void CyrusBeckSample()
{
	Scene::SetBackground(Palette::Black);

	auto hexagon = Shape2D::Hexagon(200, Scene::Size() / 2.0).asPolygon().outline(CloseRing::Yes);


	auto ComputeCyrusBeck = [&](const LineString& object, Line line, Line& result)
		{
			// 多角形の頂点を抽出
			int lineCount = object.num_lines(CloseRing::Yes);
			Array<Line> objectLines; objectLines.resize(lineCount);
			Array<Vec2> objectNormals; objectNormals.resize(lineCount);
			for (int i = 0; i < lineCount; i++)
			{
				objectLines[i] = object.line(i, CloseRing::Yes);
				objectNormals[i] = -objectLines[i].normal();
			}

			// P1-P0
			Vec2 P10 = line.end - line.begin;

			Vec2 tempT = { 0.0, 1.0 };
			for (int i = 0; auto& objectLine : objectLines)
			{
				// PE-P0
				Vec2 pE0 = objectLine.begin - line.begin;

				double numerator = Dot(objectNormals[i], pE0);
				double denominator = Dot(objectNormals[i], P10);

				// 各辺でtを計算して更新
				double t = numerator / denominator;
				if (denominator > 0)
				{
					tempT.x = Max(tempT.x, t);
				}
				else
				{
					tempT.y = Min(tempT.y, t);
				}

				i++;
			}

			// 逆転しているのは外側と等しいので打ち切り
			if (tempT.x > tempT.y) { return false; }

			result.begin = line.begin + P10 * tempT.x;
			result.end = line.begin + P10 * tempT.y;

			return true;
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
		// ヘキサゴンを描画
		hexagon.draw(Palette::Blue);

		if (KeyEnter.pressed())
		{
			if (beforeLines.size() > 0)
			{
				Line line = beforeLines.back(); beforeLines.pop_back();
				Line result;

				if (ComputeCyrusBeck(hexagon, line, result))
				{
					afterLinesClip.push_back(result);
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
