#pragma once
#include "Siv3D.hpp"

void PentagonEdgeFunctionSmaple()
{
	Image image(Scene::Size(), Palette::Black);

	DynamicTexture texture(image);

	enum class EdgeResult
	{
		Right, Left, OnLine
	};

	auto EdgeFunction = [](Point p, Point origin, float dx, float dy)
		{
			float edge = (p.x - origin.x) * dy - (p.y - origin.y) * dx;

			if (Abs(edge) < 0.00001f) { return EdgeResult::OnLine; }

			return edge > 0 ? EdgeResult::Right : EdgeResult::Left;
		};

	// 点
	Point p0 = { 300,100 };
	Point p1 = { 600,200 };
	Point p2 = { 500,400 };
	Point p3 = { 300,600 };
	Point p4 = { 100,200 };

	auto DrawTriangleSimple = [&]()
		{
			float dx01 = p0.x - p1.x;
			float dy01 = p0.y - p1.y;
			float dx12 = p1.x - p2.x;
			float dy12 = p1.y - p2.y;
			float dx23 = p2.x - p3.x;
			float dy23 = p2.y - p3.y;
			float dx34 = p3.x - p4.x;
			float dy34 = p3.y - p4.y;
			float dx40 = p4.x - p0.x;
			float dy40 = p4.y - p0.y;

			for (int x = 0; x < image.width(); x++)
			{
				for (int y = 0; y < image.height(); y++)
				{
					Point p{ x,y };
					if (EdgeFunction(p, p0, dx01, dy01) == EdgeResult::Left) { continue; }
					if (EdgeFunction(p, p1, dx12, dy12) == EdgeResult::Left) { continue; }
					if (EdgeFunction(p, p2, dx23, dy23) == EdgeResult::Left) { continue; }
					if (EdgeFunction(p, p3, dx34, dy34) == EdgeResult::Left) { continue; }
					if (EdgeFunction(p, p4, dx40, dy40) == EdgeResult::Left) { continue; }

					image[y][x] = Palette::White;
				}
			}
		};

	DrawTriangleSimple();
	texture.fill(image);

	while (System::Update())
	{
		texture.draw();
	}
}
