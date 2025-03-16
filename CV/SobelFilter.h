#pragma once
#include <Siv3D.hpp>

void SobelFilterSample()
{
	auto MakeImage = [&]()
		{
			Image image{ U"example/windmill.png" };

			Image resultImage = image;

			int filterSize = 1;

			auto filterProcess = [&](int w, int h)
				{
					Vec3 sum = Vec3::Zero();

					Vec3 result = Vec3::Zero();

					// 横方向なので、[[-1,0,1],[-2,0,2],[-1,0,1]]の形式
					sum = static_cast<ColorF>(image[h - 1][w - 1]).rgb() * -1.0;
					sum += static_cast<ColorF>(image[h - 1][w + 1]).rgb() * 1.0;
					sum += static_cast<ColorF>(image[h][w - 1]).rgb() * -2.0;
					sum += static_cast<ColorF>(image[h][w + 1]).rgb() * 2.0;
					sum += static_cast<ColorF>(image[h + 1][w - 1]).rgb() * -1.0;
					sum += static_cast<ColorF>(image[h + 1][w + 1]).rgb() * 1.0;

					/*
					// もし縦方向なら[[1,2,1][0,0,0],[-1,-2,-1]
					
					sum = static_cast<ColorF>(image[h - 1][w - 1]).rgb() * 1.0;
					sum += static_cast<ColorF>(image[h - 1][w]).rgb() * 2.0;
					sum += static_cast<ColorF>(image[h - 1][w + 1]).rgb() * 1.0;
					sum += static_cast<ColorF>(image[h + 1][w - 1]).rgb() * -1.0;
					sum += static_cast<ColorF>(image[h + 1][w]).rgb() * -2.0;
					sum += static_cast<ColorF>(image[h + 1][w + 1]).rgb() * -1.0;
					*/

					// 重みを相殺しておく
					result = sum / 8.0;

					resultImage[h][w] = { ColorF(result, 1.0f) };
				};

			int32 width = image.width();
			int32 height = image.height();

			for (int w = 0; w < width; w++)
			{
				for (int h = 0; h < height; h++)
				{
					if (w < filterSize || width - filterSize <= w ||
						h < filterSize || height - filterSize <= h)
					{
						resultImage[h][w] = Palette::Black;
						continue;
					}
					filterProcess(w, h);
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
