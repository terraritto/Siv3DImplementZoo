#pragma once
#include <Siv3D.hpp>

void JuliaSample()
{
	auto Julia = [](double x, double y)
	{
		// Julia集合の場合はCが固定
		constexpr double A = -0.3, B = -0.63;

		double a = x, b = y;
		for (int32 n = 0; n < 360; ++n)
		{
			// 実部 x(n+1) = x(n)^2 - y(n)^2 + a
			const double t = (a * a - b * b + A);
			// 虚部 y(n+1) = 2*x(n)*y(n) + b
			const double u = (2.0 * a * b + B);

			// 発散は |z_m| > 2 で判定可能
			// 発散する場合はその収束の速度を表示する
			if (4.0 < (t * t + u * u))
			{
				return n;
			}

			a = t;
			b = u;
		}

		return 0;
	};

	constexpr Size resolution{ 640, 480 };
	Window::Resize(resolution);

	Vec2 center(0, 0);
	double scale = -4.0;

	// 結果を保存する画像
	Image image{ resolution, Palette::Black };

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
			const double d = (1.0 / s) / resolution.x;

			if (clicked)
			{
				center += (Cursor::PosF() - resolution / 2) * d;
			}

			const double xb = center.x - d * (resolution.x * 0.5);
			const double yb = center.y - d * (resolution.y * 0.5);

			for (auto y : step(resolution.y))
			{
				const double yPos = yb + (d * y);

				for (auto x : step(resolution.x))
				{
					const double xPos = xb + (d * x);

					// 単純にPixel毎に判定するだけ
					if (const int32 m = Julia(xPos, yPos))
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
