#pragma once
#include <Siv3D.hpp>

void BayarFilterSample()
{
	auto MakeImage = [&]()
		{
			Image image{ U"example/windmill.png" };

			Image tempImage = image;

			Image resultImage = image;

			auto filterProcess = [&](int w, int h)
				{
					Vec3 result = static_cast<ColorF>(image[h][w]).rgb();

					int wIndex = w % 2;
					int hIndex = h % 2;

					int rgbIndex = 1;
					if (hIndex == 0 && wIndex == 0) { rgbIndex = 2; }
					else if (hIndex == 1 && wIndex == 1) { rgbIndex = 0; }

					result.x = rgbIndex == 0 ? result.x : 0;
					result.y = rgbIndex == 1 ? result.y : 0;
					result.z = rgbIndex == 2 ? result.z : 0;

					tempImage[h][w] = { ColorF(result, 1.0f) };
				};

			auto demosaicProcess = [&](int w, int h)
				{
					Vec3 result = static_cast<ColorF>(tempImage[h][w]).rgb();

					Vec3 first = static_cast<ColorF>(tempImage[h - 1][w]).rgb();
					first += static_cast<ColorF>(tempImage[h + 1][w]).rgb();
					first += static_cast<ColorF>(tempImage[h][w - 1]).rgb();
					first += static_cast<ColorF>(tempImage[h][w + 1]).rgb();
					first /= 4.0;

					Vec3 second = static_cast<ColorF>(tempImage[h - 1][w - 1]).rgb();
					second += static_cast<ColorF>(tempImage[h + 1][w - 1]).rgb();
					second += static_cast<ColorF>(tempImage[h + 1][w + 1]).rgb();
					second += static_cast<ColorF>(tempImage[h - 1][w + 1]).rgb();
					second /= 4.0;

					int wIndex = w % 2;
					int hIndex = h % 2;

					int rgbIndex = 1;
					if (hIndex == 0 && wIndex == 0) { rgbIndex = 2; }
					else if (hIndex == 1 && wIndex == 1) { rgbIndex = 0; }

					if (rgbIndex == 0)
					{
						result.y = first.y; result.z = second.z;
					}
					else if (rgbIndex == 2)
					{
						result.x = second.x; result.y = first.y;
					}
					else
					{
						result.y = 0;
					}

					resultImage[h][w] = { ColorF(result, 1.0f) };
				};

			int32 width = image.width();
			int32 height = image.height();

			for (int w = 0; w < width; w++)
			{
				for (int h = 0; h < height; h++)
				{
					filterProcess(w, h);
				}
			}

			for (int w = 0; w < width; w++)
			{
				for (int h = 0; h < height; h++)
				{
					if (w < 1 || width - 1 <= w ||
					h < 1 || height - 1 <= h)
					{
						resultImage[h][w] = Palette::Black;
						continue;
					}

					demosaicProcess(w, h);
				}
			}

			return resultImage;
		};


	Image image = MakeImage();
	const Texture texture{ image };

	while (System::Update())
	{
		texture.draw();
	}
}
