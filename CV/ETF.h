#pragma once
#include <Siv3D.hpp>

void ETF()
{

	int filterSize = 2;
	const double Eta = 1.0;
	const int Iteration = 3;
	const int DrawOffset = 15;
	const double LineScale = 10.0;

	// Sobel Util
	const Array<Array<double>> weightsY{
	{-2,-2,-4,-2,-2},
	{-1,-1,-2,-1,-1},
	{0,0,0,0,0},
	{1,1,2,1,1},
	{2,2,4,2,2}
	};
	const Array<Array<double>> weightsX{
	{-2,-1,0,1,2},
	{-2,-1,0,1,2},
	{-4,-2,0,2,4},
	{-2,-1,0,1,2},
	{-2,-1,0,1,2}
	};

	auto SobelFilter = [&](const Array<Array<double>>& p, const Array<Array<double>>& weights)
		{
			const int imageWidth = p[0].size();
			const int imageHeight = p.size();

			Array<Array<double>> result;
			result.resize(imageHeight);
			for (int i = 0; i < result.size(); i++) { result[i].resize(imageWidth, 0.0); }

			for (int i = 0; i < imageWidth; i++)
			{
				for (int j = 0; j < imageHeight; j++)
				{
					double sum = 0.0;
					for (int w = -2; w <= 2; w++)
					{
						for (int h = -2; h <= 2; h++)
						{
							const int width = Clamp(i + w, 0, imageWidth - 1);
							const int height = Clamp(j + h, 0, imageHeight - 1);
							const double weight = weights[h + 2][w + 2];
							sum += p[height][width] * weight;
						}
					}

					result[j][i] = sum;
				}
			}

			return result;
		};

	auto MakeImage = [&]()
		{
			Image image{ U"example/castle_01.jpeg" }; image.grayscale();
			Window::Resize(image.size());
			int32 width = image.width();
			int32 height = image.height();

			auto resultImage = image;

			Array<Array<double>> g; Array<Array<Vec2>> f;
			g.resize(height); f.resize(height);
			for (int i = 0; i < g.size(); i++)
			{
				g[i].resize(width, 0.0);
				f[i].resize(width, Vec2::Zero());
			}

			Array<Array<double>> gx = g, gy = g;
			auto f_next = f;

			auto filterProcess = [&](int w, int h)
				{
					Vec2 sum = Vec2::Zero();
					for (int i = -filterSize; i <= filterSize; i++)
					{
						for (int j = -filterSize; j <= filterSize; j++)
						{
							double r = (filterSize * 2.0 + 1.0) / 2.0;
							Vec2 current{ w, h };
							Vec2 next{ Clamp(w + j, 0, image.width() - 1), Clamp(h + i, 0, image.height() - 1) };

							// ws
							double ws = (next-current).length() < r ? 1.0 : 0.0;

							// wm
							double wm = 0.5 * (1.0 + Tanh(Eta * (g[next.y][next.x] - g[current.y][current.x])));

							// wd
							double tot = Dot(f[next.y][next.x], f[current.y][current.x]);
							double wd = Abs(tot);

							// phi
							double phi = tot > 0 ? 1.0 : -1.0;

							sum += phi * f[next.y][next.x] * ws * wm * wd;
						}
					}

					f_next[h][w] = sum.normalized();
				};

			// w_mのための用意
			{
				for (int w = 0; w < width; w++)
				{
					for (int h = 0; h < height; h++)
					{
						g[h][w] = image[h][w].r;
					}
				}

				// ソーベル
				gx = SobelFilter(g, weightsX);
				gy = SobelFilter(g, weightsY);

				// 正規化
				for (int w = 0; w < width; w++)
				{
					for (int h = 0; h < height; h++)
					{
						double x = gx[h][w];
						double y = gy[h][w];
						g[h][w] = Sqrt(x * x + y * y);

						// tの初期化
						Vec2 t{ x,y };
						t.normalize(); // 正規化して
						t.rotate90(-1); // 90度回転！
						f[h][w] = t;
					}
				}
			}

			// フィルタ処理
			for (int i = 0; i < Iteration; i++)
			{
				for (int w = 0; w < width; w++)
				{
					for (int h = 0; h < height; h++)
					{
						filterProcess(w, h);
					}
				}

				// next
				f = f_next;
			}

			// 矢印を描画
			for (int w = 0; w < width; w+=DrawOffset)
			{
				for (int h = 0; h < height; h+=DrawOffset)
				{
					Vec2 start{ w,h };
					Vec2 end = start + f[h][w] * LineScale;

					Line{ start, end }.paintArrow(resultImage, 1, Vec2{ 5,5 }, Palette::Red);
				}
			}

			return resultImage;
		};

	DynamicTexture texture{ MakeImage() };

	while (System::Update())
	{
		texture.draw();
	}
}
