#pragma once
#include <Siv3D.hpp>

void LoGFilter()
{
	int filterSize = 5;
	double sigma = 2.0;

	auto MakeImage = [&]()
		{
			Image image{ U"example/landscape.jpeg" };
			Window::Resize(image.size());
			int32 width = image.width();
			int32 height = image.height();
			
			Array<Array<double>> resultImage(height, Array<double>(width, 0.0));

			auto filterProcess = [&](int w, int h)
				{
					double sum = 0.0;

					double result = 0.0;
					for (int i = -filterSize; i <= filterSize; i++)
					{
						for (int j = -filterSize; j <= filterSize; j++)
						{
							int filterW = Clamp(w + j, 0, image.width() - 1);
							int filterH = Clamp(h + i, 0, image.height() - 1);

							double next = image[filterH][filterW].grayscale(); // f(i+m, j+n)

							// 重み計算
							double sigma2 = sigma * sigma;
							double sigma4 = sigma2 * sigma2;

							double factor1 = (i * i + j * j) - 2.0 * sigma2; // 分子
							double factor2 = 2.0 * Math::Pi * sigma4 * sigma2; // 分母
							double factor = factor1 / factor2;

							double exp1 = Exp(-(i * i + j * j) / (2.0f * sigma2)); // exp

							double conv = factor * exp1; // 合成

							result += conv * next;
							sum += conv;
						}
					}

					result = result / sum;
					resultImage[h][w] = result;
				};

			auto ZeroCrossProcess = [&](int w, int h)
				{
					double current = resultImage[h][w];
					double minLoG = 1.0e+10, maxLoG = -1.0e+10;
					for (int i = -1; i <= 1; i++)
					{
						for (int j = -1; j <= 1; j++)
						{
							int filterW = Clamp(w + j, 0, image.width() - 1);
							int filterH = Clamp(h + i, 0, image.height() - 1);

							double next = resultImage[filterH][filterW];

							// 最小最大を求める
							minLoG = Min(minLoG, next);
							maxLoG = Max(maxLoG, next);
						}
					}

					bool isCross = minLoG < 0.0 && current > 0.0;
					isCross |= maxLoG > 0.0 && current <= 0.0;

					image[h][w] = ColorF(Vec3::One() * static_cast<double>(isCross), 1.0);
				};

			// フィルタ処理
			for (int w = 0; w < width; w++)
			{
				for (int h = 0; h < height; h++)
				{
					filterProcess(w, h);
				}
			}

			// Zero-Cross
			for (int w = 0; w < width; w++)
			{
				for (int h = 0; h < height; h++)
				{
					ZeroCrossProcess(w, h);
				}
			}

			return image;
		};


	Image image = MakeImage();
	const DynamicTexture texture{ image };

	while (System::Update())
	{
		texture.draw();
	}
}
