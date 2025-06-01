#include "BresenhamDrawMethod.h"

namespace TerakoyaRenderer
{
	void BresenhamDrawMethod::Draw(const Vec2& v1, const Vec2& v2, const Vec2& v3, Image& image)
	{
		DrawBresenham(v1, v2, image);
		DrawBresenham(v2, v3, image);
		DrawBresenham(v3, v1, image);
	}

	void BresenhamDrawMethod::DrawBresenham(const Vec2& v1, const Vec2& v2, Image& image)
	{
		int dx = Floor(v2.x - v1.x);
		int dy = Floor(v2.y - v1.y);

		if (dx == 0 && dy == 0) { return; }

		Vector2D<int> p2Int({ static_cast<int>(Floor(v2.x)), static_cast<int>(Floor(v2.y)) });

		int diffX = dx >= 0 ? 1 : -1; diffX = Abs(v2.x - v1.x) < 1.0f ? 0 : diffX;
		int diffY = dy >= 0 ? 1 : -1; diffY = Abs(v2.y - v1.y) < 1.0f ? 0 : diffY;

		if (diffX == 0 && diffY == 0) { return; }
		else if (diffX == 0) { p2Int.x = static_cast<int>(Floor(v1.x)); }
		else if (diffY == 0) { p2Int.y = static_cast<int>(Floor(v1.x)); }

		// プラスにしておく
		dx = abs(dx);
		dy = abs(dy);

		int x = Floor(v1.x), y = Floor(v1.y);
		int d = diffX >= diffY ? 2 * dy - dx : 2 * dx - dy;
		d = diffX == diffY ? -1 : d; // 0の場合は-1にして動かないようにする
		image[y][x] = Palette::White;

		if (dx >= dy)
		{
			while (x != p2Int.x)
			{
				x += diffX;
				if (d >= 0)
				{
					y += diffY;
					d -= 2 * dx;
				}
				d += 2 * dy;
				image[y][x] = Palette::White;

				if (diffX == 0 && p2Int.y == y)
				{
					break;
				}
			}
		}
		else
		{
			while (y != p2Int.y)
			{
				y += diffY;
				if (d >= 0)
				{
					x += diffX;
					d -= 2 * dy;
				}
				d += 2 * dx;
				image[y][x] = Palette::White;

				if (diffY == 0 && p2Int.x == x)
				{
					break;
				}
			}
		}
	}
}
