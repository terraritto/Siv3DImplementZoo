#pragma once
#include <Siv3D.hpp>

void BilateralFilterSample()
{
	int filterSize = 5;
	int iterate = 5;
	float sigma1 = 0.5f, sigma2 = 0.5f;

	auto MakeImage = [&]()
		{
			Image image{ U"example/windmill.png" };

			Image resultImage = image;

			auto filterProcess = [&](int w, int h)
				{
					Vec3 sum = Vec3::Zero();

					Vec3 result = Vec3::Zero();
					for (int i = -filterSize; i <= filterSize; i++)
					{
						for (int j = -filterSize; j <= filterSize; j++)
						{
							ColorF current = image[h][w];
							ColorF next = image[h + i][w + j]; // f(i+m, j+n)

							// 重み計算
							float exp1 = Exp(-(i * i + j * j) / (2.0f * sigma1 * sigma1));
							Vec3 p = current.rgb() - next.rgb();
							Vec3 exp2 = Exp(-(p * p) / (2.0f * sigma2 * sigma2));
							exp2 *= exp1; // これがw(i,j,m,n)

							result += exp2 * next.rgb();
							sum += exp2;
						}
					}

					result = result / sum;
					resultImage[h][w] = { ColorF(result, 1.0f) };
				};

			int32 width = image.width();
			int32 height = image.height();

			for (int i = 0; i < iterate; i++)
			{
				image = resultImage;
				for (int w = filterSize; w < width - filterSize; w++)
				{
					for (int h = filterSize; h < height - filterSize; h++)
					{
						filterProcess(w, h);
					}
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
