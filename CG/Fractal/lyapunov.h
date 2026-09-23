#pragma once
#include <Siv3D.hpp>
#include <complex>

void LyapunovSample()
{
	// AABAB
	constexpr std::array<int, 5> sequence {1, 1, 0, 1, 0};
	constexpr int MaxIter = sequence.size();

	auto lyapunov = [&](double a, double b)
	{
		const int loop = 1000;
		double x0 = 0.5;
		double result = 0.0;

		for (int32 n = 0; n < loop; ++n)
		{
			for (int i = 0; i < MaxIter; i++)
			{
				// rnをAABABの周期で選択
				double rn = sequence[i] == 1 ? a : b;

				// 計算
				double xn = rn * x0 * (1.0 - x0);
				double rr = Log(Abs(rn * (1.0 - 2.0 * xn)));
				result += IsInfinity(rr) || IsNaN(rr) ? 0.0 : rr;

				x0 = xn;
			}
		}

		return result / static_cast<double>(loop * MaxIter);
	};

	constexpr Size resolution{ 640, 480 };
	Window::Resize(resolution);

	// 結果を保存する画像
	Image image{ resolution, Palette::Black };

	// 描画用の動的テクスチャ
	DynamicTexture texture(image);

	while (System::Update())
	{

		// 最初のフレームのときだけ更新
		if (Scene::FrameCount() == 1)
		{

			for (int i = 0; i < resolution.x; i++)
			{
				double a = Clamp(static_cast<double>(i) / resolution.x * 4.0, 0.0, 4.0);
				for (int j = 0; j < resolution.y; j++)
				{
					double b = Clamp(static_cast<double>(j) / resolution.y * 4.0, 0.0, 4.0);

					// 計算
					auto lm = lyapunov(a, b);

					// 色付け
					if (lm > 0)
					{
						double value = Clamp(lm, 0.0, 1.0);
						image[j][i] = ColorF(1.0, value, value);
					}
					else
					{
						double value = Clamp(Exp(lm), 0.0, 1.0);
						image[j][i] = ColorF(value, value, 1.0);
					}
				}
			}

			// 動的テクスチャの中身を image で更新
			texture.fill(image);
		}

		// テクスチャを描画
		texture.draw();
	}
}
