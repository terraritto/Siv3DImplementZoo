#pragma once
#include "Siv3D.hpp"

void BresenhamSample()
{
	Image image(Scene::Size(), Palette::Black);

	DynamicTexture texture(image);

	std::queue<Point> clickQueue;

	// 論文通り
	auto Bresenham = [&image](Point p1, Point p2)
		{
			int dx = p2.x - p1.x;
			int dy = p2.y - p1.y;

			int d = 2 * dy - dx;
			image[p1.y][p1.x] = Palette::White;
			
			int y = p1.y;
			for (int x = p1.x; x <= p2.x; x++)
			{
				if (d > 0)
				{
					y = y + 1;
					image[y][x] = Palette::White;
					d = d + (2 * dy - 2 * dx);
				}
				else
				{
					image[y][x] = Palette::White;
					d = d + (2 * dy);
				}
			}
		};

	// 論文の8象限対応したもの
	auto BresenhamAll = [&image](Point p1, Point p2)
		{
			int dx = p2.x - p1.x;
			int dy = p2.y - p1.y;
			int diffX = dx >= 0 ? 1 : -1;
			int diffY = dy >= 0 ? 1 : -1;

			// プラスにしておく
			dx = abs(dx);
			dy = abs(dy);

			int x = p1.x, y = p1.y;
			int d = diffX >= diffY ? 2 * dy - dx : 2 * dx - dy;
			image[y][x] = Palette::White;

			if (dx >= dy)
			{
				while (x != p2.x)
				{
					x += diffX;
					if (d >= 0)
					{
						y += diffY;
						d -= 2 * dx;
					}
					d += 2 * dy;
					image[y][x] = Palette::White;
				}
			}
			else
			{
				while (y != p2.y)
				{
					y += diffY;
					if (d >= 0)
					{
						x += diffX;
						d -= 2 * dy;
					}
					d += 2 * dx;
					image[y][x] = Palette::White;
				}
			}
		};

	while (System::Update())
	{
		texture.draw();
		if (SimpleGUI::Button(U"Make", Vec2::Zero() * 50))
		{
			if (clickQueue.size() != 2) { continue; }

			Point first = clickQueue.front(); clickQueue.pop();
			Point second = clickQueue.front(); clickQueue.pop();

			BresenhamAll(first, second);

			texture.fill(image);
		}
		else if (MouseL.up())
		{
			clickQueue.push(Cursor::Pos());

			// 2個を保証
			if (clickQueue.size() == 3)
			{
				clickQueue.pop();
			}
		}
	}
}
