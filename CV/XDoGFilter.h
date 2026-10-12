#pragma once
#include <Siv3D.hpp>

void XDoGFilter()
{
	enum class State
	{
		threshold,
		XDoG,
		Sharpness
	};

	int filterSize = 2;
	double sigma = 0.5;
	double K = 1.05;
	double epsilon = 0.5;
	double tau = 0.9;
	double phi = 10.0;
	double p = 100;

	auto MakeImage = [&](State state)
		{
			Image image{ U"example/ice_01.jpeg" };
			Window::Resize(image.size());
			int32 width = image.width();
			int32 height = image.height();

			auto resultImage = image;

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

							// sigmaを用意
							double sigmaK = sigma * K;
							double sigma2 = sigma * sigma;
							double sigma2K = sigmaK * sigmaK;

							// SigmaのGaussian
							double exp1 = Exp(-(i * i + j * j) / (2.0f * sigma2));
							double denom1 = 2.0 * Math::Pi * sigma2;
							double conv = exp1 / denom1;

							// Sigma*KのGaussian
							double denomK = 2.0 * Math::Pi * sigma2K;
							double expK = Exp(-(i * i + j * j) / (2.0f * sigma2K));
							double convK = expK / denomK;

							// DoGの重みへ
							double convResult = 0.0;
							switch (state)
							{
							case State::threshold:
								convResult = conv - convK;
								break;
							case State::XDoG:
								convResult = conv - convK;
								convResult = (1 - tau) * conv + tau * convResult;
								break;
							case State::Sharpness:
								convResult = (1.0 + p) * conv - p * convResult;
								break;
							}

							result += convResult * next;
							sum += convResult;
						}
					}

					result = result / sum;

					switch (state)
					{
					case State::threshold:
						// step function
						result = static_cast<double>(result > epsilon);
						break;

					case State::XDoG:
					case State::Sharpness:
						double t = 1.0 + Tanh(phi * (result - epsilon));
						result = result > epsilon ? 1.0 : t;
						break;
					}

					resultImage[h][w] = ColorF(Vec3::One() * result, 1.0);
				};

			// フィルタ処理
			for (int w = 0; w < width; w++)
			{
				for (int h = 0; h < height; h++)
				{
					filterProcess(w, h);
				}
			}

			return resultImage;
		};

	std::map<State, Image> imageMap;
	imageMap[State::threshold] = MakeImage(State::threshold);
	imageMap[State::XDoG] = MakeImage(State::XDoG);
	imageMap[State::Sharpness] = MakeImage(State::Sharpness);

	size_t index = 2;
	DynamicTexture texture{ imageMap[static_cast<State>(index)]};


	Array<String> radioTexts{ U"Threshold", U"XDoG", U"Sharpness" };

	while (System::Update())
	{
		texture.draw();

		if (SimpleGUI::RadioButtons(index, radioTexts, Vec2{ 500,100 }))
		{
			State state = static_cast<State>(index);
			texture.fill(imageMap[state]);
		}
	}
}
