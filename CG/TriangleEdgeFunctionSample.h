#pragma once
#include "Siv3D.hpp"

void TriangleEdgeFunctionSmaple()
{
	Image image(Scene::Size(), Palette::Black);

	DynamicTexture texture(image);

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
			d = diffX == diffY ? -1 : d; // 0の場合は-1にして動かないようにする
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

	enum class EdgeResult
	{
		Right, Left, OnLine
	};

	auto CalculateEdge = [](Point p, Point origin, float dx, float dy)
		{
			return (p.x - origin.x) * dy - (p.y - origin.y) * dx;
		};

	auto EdgeFunction = [&](Point p, Point origin, float dx, float dy)
		{
			float edge = CalculateEdge(p, origin, dx, dy);

			if (Abs(edge) < 0.00001f) { return EdgeResult::OnLine; }

			return edge > 0 ? EdgeResult::Right : EdgeResult::Left;
		};

	// 点
	Point p0 = { 300,100 };
	Point p1 = { 500,400 };
	Point p2 = { 100,200 };

	// Bounding Box
	Point BoundingX = { Min({p0.x,p1.x,p2.x }), Max({ p0.x,p1.x,p2.x }) };
	Point BoundingY = { Min({p0.y,p1.y,p2.y }), Max({ p0.y,p1.y,p2.y }) };

	int count_01 = 0;
	int count_02 = 0;

	auto DrawTriangleSimple = [&]()
		{
			float dx01 = p0.x - p1.x;
			float dy01 = p0.y - p1.y;
			float dx12 = p1.x - p2.x;
			float dy12 = p1.y - p2.y;
			float dx20 = p2.x - p0.x;
			float dy20 = p2.y - p0.y;

			// パターン1:愚直に埋める
			/*
			for (int x = 0; x < image.width(); x++)
			{
				for (int y = 0; y < image.height(); y++)
				{
					Point p{ x,y };
					if (EdgeFunction(p, p0, dx01, dy01) == EdgeResult::Left) { continue; }
					if (EdgeFunction(p, p1, dx12, dy12) == EdgeResult::Left) { continue; }
					if (EdgeFunction(p, p2, dx20, dy20) == EdgeResult::Left) { continue; }

					image[y][x] = Palette::White;
				}
			}
			*/

			// パターン2:BBoxで埋めていく
			for (int x = BoundingX.x; x <= BoundingX.y; x++)
			{
				for (int y = BoundingY.x; y <= BoundingY.y; y++)
				{
					count_01++;
					/*
					Point p{ x,y };
					if (EdgeFunction(p, p0, dx01, dy01) == EdgeResult::Left) { continue; }
					if (EdgeFunction(p, p1, dx12, dy12) == EdgeResult::Left) { continue; }
					if (EdgeFunction(p, p2, dx20, dy20) == EdgeResult::Left) { continue; }

					image[y][x] = Palette::White;
					*/
				}
			}

			// パターン3:賢く埋めていく
			Point minPoint{ 0,99999 }; // 最小点を得る
			if (minPoint.y > p0.y) { minPoint = p0; }
			if (minPoint.y > p1.y) { minPoint = p1; }
			if (minPoint.y > p2.y) { minPoint = p2; }

			int dd = -1;
			int base = -1, baseCache = -1;
			bool isFirst = true;

			for (int y = BoundingY.x; y <= BoundingY.y; y++)
			{
				Point BasePoint{ minPoint.x, y };

				// Baseについて先に計算して内外を決めておく
				bool isInside = true;
				if (EdgeFunction(BasePoint, p0, dx01, dy01) == EdgeResult::Left) { isInside = false; }
				if (EdgeFunction(BasePoint, p1, dx12, dy12) == EdgeResult::Left) { isInside = false; }
				if (EdgeFunction(BasePoint, p2, dx20, dy20) == EdgeResult::Left) { isInside = false; }

				if (isInside)
				{
					int dxLeft = 1, dxRight = 1;
					bool isFinishLeft = false, isFinishRight = false;

					while (true)
					{
						if (!isFinishLeft)
						{
							image[BasePoint.y][BasePoint.x] = Palette::White;
							count_02++;

							Point leftPoint{ BasePoint.x - dxLeft, BasePoint.y };
							if (EdgeFunction(leftPoint, p0, dx01, dy01) == EdgeResult::Left) { isFinishLeft = true; }
							if (EdgeFunction(leftPoint, p1, dx12, dy12) == EdgeResult::Left) { isFinishLeft = true; }
							if (EdgeFunction(leftPoint, p2, dx20, dy20) == EdgeResult::Left) { isFinishLeft = true; }

							// まだ中なら処理
							if (!isFinishLeft)
							{
								image[leftPoint.y][leftPoint.x] = Palette::White;
								dxLeft++;
								count_02++;
							}
						}

						if (!isFinishRight)
						{
							Point rightPoint{ BasePoint.x + dxRight, BasePoint.y };
							if (EdgeFunction(rightPoint, p0, dx01, dy01) == EdgeResult::Left) { isFinishRight = true; }
							if (EdgeFunction(rightPoint, p1, dx12, dy12) == EdgeResult::Left) { isFinishRight = true; }
							if (EdgeFunction(rightPoint, p2, dx20, dy20) == EdgeResult::Left) { isFinishRight = true; }

							// まだ中なら処理
							if (!isFinishRight)
							{
								image[rightPoint.y][rightPoint.x] = Palette::White;
								dxRight++;
								count_02++;
							}
						}

						// 両方出てたら終了
						if (isFinishLeft && isFinishRight) { break; }
					}
				}
				else
				{
					int dxLeft = 1, dxRight = 1;
					int isLeftDir = false;

					if (isFirst)
					{
						while (true)
						{
							// まずは中を探す

							// 左
							bool isLeft = false;
							Point leftPoint{ BasePoint.x - dxLeft, BasePoint.y };
							if (EdgeFunction(leftPoint, p0, dx01, dy01) == EdgeResult::Left) { isLeft = true; }
							if (EdgeFunction(leftPoint, p1, dx12, dy12) == EdgeResult::Left) { isLeft = true; }
							if (EdgeFunction(leftPoint, p2, dx20, dy20) == EdgeResult::Left) { isLeft = true; }
							count_02++;

							if (isLeft)
							{
								// まだ外
								image[leftPoint.y][leftPoint.x] = Palette::Red;
								dxLeft++;
							}
							else
							{
								// 中なら終了
								image[leftPoint.y][leftPoint.x] = Palette::White;
								dxLeft++;
								isLeftDir = true;
								break;
							}

							// 右
							bool isRight = false;
							Point rightPoint{ BasePoint.x + dxRight, BasePoint.y };
							if (EdgeFunction(rightPoint, p0, dx01, dy01) == EdgeResult::Left) { isRight = true; }
							if (EdgeFunction(rightPoint, p1, dx12, dy12) == EdgeResult::Left) { isRight = true; }
							if (EdgeFunction(rightPoint, p2, dx20, dy20) == EdgeResult::Left) { isRight = true; }
							count_02++;

							if (isRight)
							{
								// まだ外
								image[rightPoint.y][rightPoint.x] = Palette::Red;
								dxRight++;
							}
							else
							{
								// 中なら終了
								image[rightPoint.y][rightPoint.x] = Palette::White;
								dxRight++;
								isLeftDir = false;
								break;
							}
						}

						// 確定したものを保持しておく
						dd = isLeftDir ? -1 : 1;
						base = isLeftDir ? dxLeft : dxRight;
						baseCache = base - 1;
					}
					else
					{
						base = baseCache;
						// 内に入るまでやる
						while (true)
						{
							bool isOutside = false;
							Point pp{ BasePoint.x + base * dd, BasePoint.y };
							if (EdgeFunction(pp, p0, dx01, dy01) == EdgeResult::Left) { isOutside = true; }
							if (EdgeFunction(pp, p1, dx12, dy12) == EdgeResult::Left) { isOutside = true; }
							if (EdgeFunction(pp, p2, dx20, dy20) == EdgeResult::Left) { isOutside = true; }
							count_02++;

							// まだ外なら処理
							if (isOutside)
							{
								image[pp.y][pp.x] = Palette::Red;
								base++;
							}
							else
							{
								image[pp.y][pp.x] = Palette::White;
								baseCache = base - 1;
								break;
							}
						}
					}

					// 外に出るまでやる
					bool isOutside = false;
					while (true)
					{
						Point pp{ BasePoint.x + base * dd, BasePoint.y };
						if (EdgeFunction(pp, p0, dx01, dy01) == EdgeResult::Left) { isOutside = true; }
						if (EdgeFunction(pp, p1, dx12, dy12) == EdgeResult::Left) { isOutside = true; }
						if (EdgeFunction(pp, p2, dx20, dy20) == EdgeResult::Left) { isOutside = true; }
						count_02++;

						// まだ中なら処理
						if (!isOutside)
						{
							image[pp.y][pp.x] = Palette::White;
							base++;
						}
						else
						{
							break;
						}
					}

					isFirst = false;
				}
			}
		};

	DrawTriangleSimple();

	// 四角形を描いておく
	BresenhamAll(Point{ BoundingX.x, BoundingY.x }, Point{ BoundingX.y, BoundingY.x });
	BresenhamAll(Point{ BoundingX.x, BoundingY.y }, Point{ BoundingX.y, BoundingY.y });
	BresenhamAll(Point{ BoundingX.x, BoundingY.x }, Point{ BoundingX.x, BoundingY.y });
	BresenhamAll(Point{ BoundingX.y, BoundingY.x }, Point{ BoundingX.y, BoundingY.y });
	
	texture.fill(image);

	while (System::Update())
	{
		texture.draw();

		ClearPrint();

		Print << U"All Pixel: " << (image.width() * image.height());
		Print << U"BBox: " << count_01;
		Print << U"Optimize: " << count_02;
	}
}
