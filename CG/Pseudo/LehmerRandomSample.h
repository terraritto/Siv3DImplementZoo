#pragma once
#include <Siv3D.hpp>

void LehmerRandomSample()
{
	Window::Resize(512, 256);

	// a = 7^5 = 16807
	// m = 2^31 - 1 = 2147483647, メルセンヌ素数のためxと互いに素になりにくい
	constexpr long long int a = 16807;
	constexpr long long m = 2147483647;

	long long int x_n = 10000; // 初期値
	auto step = [&]()
		{
			// x_(n+1) = a * x_n mod m
			x_n = (a * x_n) % m;
			return x_n;
		};

	Image image{ Scene::Size() };

	for (int n = 0; n < 100000; n++)
	{
		int x = step() % image.width();
		int y = step() % image.height();

		int color = step();
		int r = step() % 255;
		int g = step() % 255;
		int b = step() % 255;

		image[y][x] = Color(r, g, b);
	}

	RenderTexture tex{ image };

	while (System::Update())
	{
		tex.draw();
	}
}
