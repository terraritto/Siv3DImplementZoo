#pragma once
#include <Siv3D.hpp>

void MandelbrotSample()
{
	auto Mandelbrot = [](double a, double b)
	{
		double x = 0.0, y = 0.0;
		for (int32 n = 0; n < 360; ++n)
		{
			// 実部 x(n+1) = x(n)^2 - y(n)^2 + a
			const double t = (x * x - y * y + a);
			// 虚部 y(n+1) = 2*x(n)*y(n) + b
			const double u = (2.0 * x * y + b);

			// 発散は |z_m| > 2 で判定可能
			// 発散する場合はその収束の速度を表示する
			if (4.0 < (t * t + u * u))
			{
				return n;
			}

			x = t;
			y = u;
		}

		return 0;
	};

	constexpr Size resolutuion{ 640, 480 };
	Window::Resize(resolutuion);

	Vec2 center(0, 0);
	double scale = -4.0;

	// 結果を保存する画像
	Image image{ resolutuion, Palette::Black };

	// 描画用の動的テクスチャ
	DynamicTexture texture(image);

	while (System::Update())
	{
		const double wheel = Mouse::Wheel();
		const bool clicked = MouseL.down();

		// 最初のフレームか、操作されたときだけ更新
		if (wheel || clicked || (Scene::FrameCount() == 1))
		{
			scale -= wheel;

			const double s = Pow(1.25, scale);
			const double d = (1.0 / s) / resolutuion.x;

			if (clicked)
			{
				center += (Cursor::PosF() - resolutuion / 2) * d;
			}

			const double xb = center.x - d * (resolutuion.x * 0.5);
			const double yb = center.y - d * (resolutuion.y * 0.5);

			for (auto y : step(resolutuion.y))
			{
				const double yPos = yb + (d * y);

				for (auto x : step(resolutuion.x))
				{
					const double xPos = xb + (d * x);

					// 単純にPixel毎に判定するだけ
					if (const int32 m = Mandelbrot(xPos, yPos))
					{
						image[y][x] = HSV{ (240 - m), 0.8, 1.0 };
					}
					else
					{
						image[y][x] = Palette::Black;
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
