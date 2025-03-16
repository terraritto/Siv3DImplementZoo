#pragma once
#include <Siv3D.hpp>

void SNNFilterSample()
{
	auto MakeImage = [&]()
		{
			Image image{ U"example/snn_test.jpg" };

			Image resultImage = image;

			int filterSize = 1;

			auto filterProcess = [&](int w, int h)
				{
					Vec3 sum = Vec3::Zero();

					Vec3 result = Vec3::Zero();

					for (int j = 0; j <= filterSize; j++)
					{
						for (int i = -filterSize; i <= filterSize; i++)
						{
							ColorF current = image[h][w];
							ColorF top = image[h - i][w - j];
							ColorF bottom = image[h + i][w + j];

							// 各部分で色とどれくらい近いかを計算
							Vec3 topDiff = (top.rgb() - current.rgb());
							float topThreshold = Dot(topDiff, topDiff);

							Vec3 bottomDiff = (bottom.rgb() - current.rgb());
							float bottomThreshold = Dot(bottomDiff, bottomDiff);

							result += topThreshold > bottomThreshold ? bottom.rgb() : top.rgb();
							sum += Vec3::One();
						}
					}

					// 重みを相殺しておく
					result = result / sum;

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
