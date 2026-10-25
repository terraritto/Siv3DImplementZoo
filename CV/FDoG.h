#pragma once
#include <Siv3D.hpp>

void FDoG()
{

	int filterSize = 2;
	const double Eta = 1.0;
	const int Iteration = 3;

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

	// FDoG
	constexpr double Sigma_L = 2;
	constexpr double Sigma_kL = 1.6 * Sigma_L; // k=1.6が論文読む限り良いらしい
	constexpr double Sigma_M = 2.0; // あまり大きいと大変なことになりそうなので小さめで
	constexpr double tau = 0.99; // 論文に掛かれていたdefault値
	constexpr double selector = 0.9; // 白黒の際の閾値
	constexpr double IterationFDoG = 3; // FDoG側の繰り返し回数

	// Debug
	constexpr bool isDrawCircle = false; // 点を表示するか
	bool isDraw = false;
	Array<Vec2> points;

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

			Array<Array<double>> gx = g, gy = g, ft = g;
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
							double ws = (next - current).length() < r ? 1.0 : 0.0;

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
						g[h][w] = image[h][w].r / 255.0;
						ft[h][w] = image[h][w].r / 255.0;
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

			// FDoGの処理
			auto PerpendicularConv = [&](Vector2D<int> pos, Vec2 t)
				{
					Vec2 n = t.rotate90(-1); // gradを形成

					if (n.length() < 1e-9) { return 0.0; }

					// それっぽく1進むように,もっと勾配を見て細かく規定してもよい
					Vector2D<int> step = Vector2D<int>::Zero();
					step.x = Sign(n.x); step.y = Sign(n.y);

					double T = Sigma_kL * 2.0;

					double sum = 0.0;
					double result = 0.0;
					for (int i = -T; i <= T; i++)
					{
						Vector2D<int> p = pos + static_cast<double>(i) * step;
						p.x = Clamp(p.x, 0, static_cast<int>(ft[0].size() - 1));
						p.y = Clamp(p.y, 0, static_cast<int>(ft.size() - 1));

						double imageValue = ft[p.y][p.x];

						// Sigmaの1D Gaussian
						double exp1 = Exp(-(pos-p).lengthSq() / (2.0f * Sigma_L * Sigma_L));
						double denom1 = Sqrt(2.0 * Math::Pi) * Sigma_L;
						double conv = exp1 / denom1;

						// Sigma*Kの1D Gaussian
						double denomK = Sqrt(2.0 * Math::Pi) * Sigma_kL;
						double expK = Exp(-(pos-p).lengthSq() / (2.0f * Sigma_kL * Sigma_kL));
						double convK = expK / denomK;

						double convResult = conv - tau * convK;

						result += imageValue * convResult;
						sum += convResult;

						if (isDrawCircle && isDraw)
						{
							if (Abs(i) == static_cast<int>(T))
							{
								points.push_back(p);
							}
						}
					}

					return result / sum;
				};

			auto FDoG = [&](Vector2D<int> pos)
				{
					auto t = f[pos.y][pos.x];

					double S = Sigma_M * 2;

					double result = 0.0, sum = 0.0;

					for (auto dir : Array<double>{1, -1})
					{
						double distance = 0.0;
						Vector2D<int> p = pos, p_prev = pos;

						while (distance < S)
						{
							Vector2D<int> step = Vector2D<int>::Zero();
							step.x = Sign(t.x); step.y = Sign(t.y);

							if (step == Vector2D<int>::Zero()) { break; }

							// 移動
							p += step * dir;

							// 範囲外は行かないようにしとく
							if (p.x < 0.0 || ft[0].size() - 1 < p.x) { break; }
							if (p.y < 0.0 || ft.size() - 1 < p.y) { break; }

							// 距離を加算
							distance += (p-p_prev).length();

							// Sigmaの1D Gaussian
							double exp1 = Exp(-distance * distance / (2.0f * Sigma_M * Sigma_M));
							double denom1 = Sqrt(2.0 * Math::Pi) * Sigma_M;
							double conv = exp1 / denom1;

							double F = PerpendicularConv(p, t);

							result += F * conv;
							sum += conv;

							// つぎの位置へ
							t = f[p.y][p.x];
							p_prev = p;

							if (isDrawCircle && isDraw)
							{
								points.push_back(p);
							}
						}
					}

					result = sum == 0.0 ? 0.0 : result / sum;
					result = result <= 0.0 && (1 + Tanh(result) < selector) ? 0.0 : 1.0;

					return result;
				};

			// フィルタ処理
			for (int i = 0; i < IterationFDoG; i++)
			{
				auto temp = ft;
				for (int w = 0; w < width; w++)
				{
					for (int h = 0; h < height; h++)
					{
						if (isDrawCircle)
						{
							isDraw = i == 0 && w == 350 && h == 100;
						}

						temp[h][w] = FDoG(Vec2{ w, h });
					}
				}

				// next
				ft = temp;
			}

			for (int w = 0; w < width; w++)
			{
				for (int h = 0; h < height; h++)
				{
					resultImage[h][w] = ColorF(ft[h][w] * Vec3::One(), 1.0);
				}
			}

			if (isDrawCircle)
			{
				for (auto& point : points)
				{
					Circle{ point, 1 }.paint(resultImage, Palette::Red);
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
