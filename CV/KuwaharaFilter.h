#pragma once
#include <Siv3D.hpp>

void KuwaharaFilterSample()
{
	int filterSize = 5;

	auto MakeImage = [&]()
		{
			Image image{ U"example/windmill.png" };

			Image resultImage = image;

			struct RectData
			{
				double sigma;
				double meanSigma;
				Vec3 mean;

				RectData() : sigma(0.0), meanSigma(0.0), mean(Vec3::Zero()) {}
			};

			// 四角部分を計算
			auto CalculateRect = [&](int w, int h, Vec2 start)
			{
					RectData data;
					// 平均
					float sum = (filterSize + 1) * (filterSize + 1);
					for (int i = start.x; i <= start.x + filterSize; i++)
					{
						for (int j = start.y; j <= start.y + filterSize; j++)
						{
							ColorF current = image[h + j][w + i];
							data.mean += current.rgb();

							HSV hsv{ current };
							data.meanSigma += hsv.v;
						}
					}
					data.mean /= sum;
					data.meanSigma /= sum;

					// 標準偏差
					for (int i = start.x; i <= start.x + filterSize; i++)
					{
						for (int j = start.y; j <= start.y + filterSize; j++)
						{
							ColorF current = image[h + j][w + i];
							HSV hsv{ current };
							double param = hsv.v - data.meanSigma;
							data.sigma += param * param;
						}
					}
					data.sigma /= sum;
					HSV hsv;
					data.sigma = Sqrt(data.sigma);

					return data;
			};

			auto filterProcess = [&](int w, int h)
				{
					Vec3 result = Vec3::Zero();

					RectData leftTop = CalculateRect(w, h, Vec2{ -filterSize, -filterSize });
					RectData RightTop = CalculateRect(w, h, Vec2{ 0, -filterSize });
					RectData leftDown = CalculateRect(w, h, Vec2{ -filterSize, 0 });
					RectData RightDown = CalculateRect(w, h, Vec2{ 0, 0 });

					auto CompData = [](RectData data, Vec3& result, float& minData)
						{
							if (data.sigma < minData)
							{
								result = data.mean;
								minData = data.sigma;
							}
						};

					float minData = 1000000.0f;
					CompData(leftTop, result, minData);
					CompData(RightTop, result, minData);
					CompData(leftDown, result, minData);
					CompData(RightDown, result, minData);

					resultImage[h][w] = { ColorF(result, 1.0f) };
				};

			int32 width = image.width();
			int32 height = image.height();

			image = resultImage;
			for (int w = filterSize; w < width - filterSize; w++)
			{
				for (int h = filterSize; h < height - filterSize; h++)
				{
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
