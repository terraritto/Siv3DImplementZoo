#pragma once
#include <Siv3D.hpp>

void GuidedFilter()
{
	constexpr int FILTER_SIZE = 3;
	constexpr int FILTER_NUM = (FILTER_SIZE * 2 + 1) * (FILTER_SIZE * 2 + 1);

	constexpr double EPSILON = 1e-1;

	Image image{ U"example/ice_01.jpeg" };
	Scene::Resize(image.size());

	auto Makeimage = [&]()
		{
			auto result = image;
			const int WIDTH = image.width(), HEIGHT = image.height();

			auto CalculateMean = [&](const int w, const int h, const Array<Array<double>>& data)
				{
					double mean = 0.0;
					for (int fw = -FILTER_SIZE; fw <= FILTER_SIZE; fw++)
					{
						for (int fh = -FILTER_SIZE; fh <= FILTER_SIZE; fh++)
						{
							int cw = Clamp(w + fw, 0, WIDTH - 1);
							int ch = Clamp(h + fh, 0, HEIGHT - 1);

							mean += data[ch][cw];
						}
					}

					return mean / FILTER_NUM;
				};

			auto CalculateRMS = [&](const int w, const int h, const Array<Array<double>>& data)
				{
					double mean = 0.0;
					for (int fw = -FILTER_SIZE; fw <= FILTER_SIZE; fw++)
					{
						for (int fh = -FILTER_SIZE; fh <= FILTER_SIZE; fh++)
						{
							int cw = Clamp(w + fw, 0, WIDTH - 1);
							int ch = Clamp(h + fh, 0, HEIGHT - 1);

							mean += data[ch][cw] * data[ch][cw];
						}
					}

					return mean / FILTER_NUM;
				};

			auto CalculateMul = [&](const int w, const int h, const Array<Array<double>>& a, const Array<Array<double>>& b)
				{
					double mean = 0.0;
					for (int fw = -FILTER_SIZE; fw <= FILTER_SIZE; fw++)
					{
						for (int fh = -FILTER_SIZE; fh <= FILTER_SIZE; fh++)
						{
							int cw = Clamp(w + fw, 0, WIDTH - 1);
							int ch = Clamp(h + fh, 0, HEIGHT - 1);

							mean += a[ch][cw] * b[ch][cw];
						}
					}

					return mean / FILTER_NUM;
				};

			Array<Array<double>> I(HEIGHT, Array<double>(WIDTH, 0.0));

			for (int w = 0; w < WIDTH; w++)
			{
				for (int h = 0; h < HEIGHT; h++)
				{
					I[h][w] = image[h][w].grayscale();
				}
			}

			auto p = I, a = I, b = I;

			for (int w = 0; w < WIDTH; w++)
			{
				for (int h = 0; h < HEIGHT; h++)
				{
					double meanI = CalculateMean(w, h, I);
					double meanP = CalculateMean(w, h, p);

					double var = CalculateRMS(w, h, I) - meanI * meanI;
					double cov = CalculateMul(w, h, I, p) - meanI * meanP;

					double a_denominator = cov;
					double a_numerator = var + EPSILON;

					a[h][w] = a_denominator / a_numerator;
					b[h][w] = meanP - a[h][w] * meanI;
				}
			}

			for (int w = 0; w < WIDTH; w++)
			{
				for (int h = 0; h < HEIGHT; h++)
				{
					double aMean = CalculateMean(w, h, a);
					double bMean = CalculateMean(w, h, b);

					double q = aMean * I[h][w] + bMean;

					result[h][w] = ColorF(Vec3::One() * q, 1.0);
				}
			}

			return result;
		};


	Image result = Makeimage();
	image.grayscale();

	bool isDefault = false;
	DynamicTexture texture{ image };

	while (System::Update())
	{
		if (KeySpace.down())
		{
			isDefault = !isDefault;
			const Image& v = isDefault ? image : result;
			texture.fill(v);
		}

		texture.draw();
	}
	
}
