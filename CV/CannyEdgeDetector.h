#pragma once
#include <Siv3D.hpp>

void CannyEdgeDetectorSample()
{
	auto MakeImage = [&]()
		{
			Image image{ U"example/windmill.png" };
			image = image.grayscale(); // グレースケールにしておく

			// ステップ1の中間バッファ
			Image GaussImage = image;

			// ステップ2の中間バッファ
			Image SobelHorizontalImage = image;
			Image SobelVerticalImage = image;

			// ステップ3の中間バッファ
			Image GradientImage = image;
			Image DirectionImage = image;

			// ステップ4の中間バッファ
			Image MaxSupressionImage = image;

			// ステップ5のバッファ
			Image ResultImage = image;
			float minThreshold = 2;
			float maxThreshold = 5;

			int filterSize = 1;

			auto GaussfilterProcess = [&](int w, int h)
				{
					Vec3 sum = Vec3::Zero();

					Vec3 result = Vec3::Zero();

					// ガウスフィルタの3x3の実装
					// [[1/16,2/16,1/16],[2/16,4/16,2/16],[1/16,2/16,1/16]]の形式
					sum = static_cast<ColorF>(image[h - 1][w - 1]).rgb() * 1.0;
					sum += static_cast<ColorF>(image[h - 1][w]).rgb() * 2.0;
					sum += static_cast<ColorF>(image[h - 1][w + 1]).rgb() * 1.0;
					sum += static_cast<ColorF>(image[h][w - 1]).rgb() * 2.0;
					sum += static_cast<ColorF>(image[h][w]).rgb() * 4.0;
					sum += static_cast<ColorF>(image[h][w + 1]).rgb() * 2.0;
					sum += static_cast<ColorF>(image[h + 1][w - 1]).rgb() * 1.0;
					sum += static_cast<ColorF>(image[h + 1][w]).rgb() * 2.0;
					sum += static_cast<ColorF>(image[h + 1][w + 1]).rgb() * 1.0;

					// 重みを相殺しておく
					result = sum / 16.0;

					GaussImage[h][w] = { ColorF(result, 1.0f) };
				};

			auto SobelHorizontalProcess = [&](int w, int h)
				{
					Vec3 sum = Vec3::Zero();

					Vec3 result = Vec3::Zero();

					// 横方向なので、[[-1,0,1],[-2,0,2],[-1,0,1]]の形式
					sum = static_cast<ColorF>(GaussImage[h - 1][w - 1]).rgb() * -1.0;
					sum += static_cast<ColorF>(GaussImage[h - 1][w + 1]).rgb() * 1.0;
					sum += static_cast<ColorF>(GaussImage[h][w - 1]).rgb() * -2.0;
					sum += static_cast<ColorF>(GaussImage[h][w + 1]).rgb() * 2.0;
					sum += static_cast<ColorF>(GaussImage[h + 1][w - 1]).rgb() * -1.0;
					sum += static_cast<ColorF>(GaussImage[h + 1][w + 1]).rgb() * 1.0;

					// 重みを相殺しておく
					result = sum / 8.0;

					SobelHorizontalImage[h][w] = { ColorF(result, 1.0f) };
				};

			auto SobelVerticalProcess = [&](int w, int h)
				{
					Vec3 sum = Vec3::Zero();

					Vec3 result = Vec3::Zero();

					// 縦方向なら[[-1,-2,-1][0,0,0],[1,2,1]

					sum = static_cast<ColorF>(GaussImage[h - 1][w - 1]).rgb() * -1.0;
					sum += static_cast<ColorF>(GaussImage[h - 1][w]).rgb() * -2.0;
					sum += static_cast<ColorF>(GaussImage[h - 1][w + 1]).rgb() * -1.0;
					sum += static_cast<ColorF>(GaussImage[h + 1][w - 1]).rgb() * 1.0;
					sum += static_cast<ColorF>(GaussImage[h + 1][w]).rgb() * 2.0;
					sum += static_cast<ColorF>(GaussImage[h + 1][w + 1]).rgb() * 1.0;

					// 重みを相殺しておく
					result = sum / 8.0;

					SobelVerticalImage[h][w] = { ColorF(result, 1.0f) };
				};

			auto GradientProcess = [&](int w, int h)
				{
					Vec3 result = Vec3::Zero();

					Vec3 X = static_cast<ColorF>(SobelHorizontalImage[h][w]).rgb();
					Vec3 Y = static_cast<ColorF>(SobelVerticalImage[h][w]).rgb();

					// sqrt(G_x^2+G_y^2)
					result = Sqrt(X * X + Y * Y);

					int r = static_cast<int>(result.x * 255.0);
					GradientImage[h][w] = Color(r, r, r);

					// atan2(G_y,G_x)
					int theta = Atan2(Y.x, X.x) / Math::Pi * 180; // [-pi, pi]->[-180, 180]
					int sign = Sign(result.x);
					int t_pos = theta * sign;
					DirectionImage[h][w] = { Color(t_pos, t_pos, t_pos, sign > 0 ? 1 : 0) };
				};

			auto NonMaximumSuprresionProcess = [&](int w, int h)
				{
					enum class Dir
					{
						Horizontal,
						DiagonalRight,
						DiagonalLeft,
						Vertical
					};

					// 方向を復元
					Dir dir;
					float direction = DirectionImage[h][w].r * (DirectionImage[h][w].a > 0.5 ? 1.0 : -1.0);
					if (direction >= -22.5 && direction < 22.5) { dir = Dir::Horizontal; }
					else if (direction >= -157.5 && direction < 180.0) { dir = Dir::Horizontal; }
					else if (direction >= -180.0 && direction < -157.5) { dir = Dir::Horizontal; }
					else if (direction >= 22.5 && direction < 67.5) { dir = Dir::DiagonalRight; }
					else if (direction >= -157.5 && direction < -112.5) { dir = Dir::DiagonalRight; }
					else if (direction >= 67.5 && direction < 112.5) { dir = Dir::Vertical; }
					else if (direction >= -112.5 && direction < -67.5) { dir = Dir::Vertical; }
					else if (direction >= 112.5 && direction < 157.5) { dir = Dir::DiagonalLeft; }
					else if (direction >= -67.5 && direction < -22.5) { dir = Dir::DiagonalLeft; }

					// 周りより大きくなければ0にする
					uint8 result = GradientImage[h][w].r;
					switch (dir)
					{
					case Dir::Horizontal:
						if (result < (GradientImage[h][w + 1]).r || result < GradientImage[h][w - 1].r)
						{
							result = 0;
						}
						break;
					case Dir::DiagonalRight:
						if (result < (GradientImage[h - 1][w + 1]).r || result < GradientImage[h][w - 1].r)
						{
							result = 0;
						}
						break;
					case Dir::DiagonalLeft:
						if (result < (GradientImage[h + 1][w + 1]).r || result < GradientImage[h - 1][w - 1].r)
						{
							result = 0;
						}
						break;
					case Dir::Vertical:
						if (result < (GradientImage[h + 1][w]).r || result < GradientImage[h - 1][w].r)
						{
							result = 0;
						}
						break;
					}
					MaxSupressionImage[h][w] = { Color(result, result, result) };
				};

			auto HysteresisThresholdProcess = [&](int w, int h)
				{
					// 周りより大きくなければ0にする
					uint8 value = MaxSupressionImage[h][w].r;

					if (value >= maxThreshold)
					{
						ResultImage[h][w] = Palette::White;
					}
					else if (value < minThreshold)
					{
						ResultImage[h][w] = Palette::Black;
					}
					else
					{
						bool isRemain = false;
						isRemain |= MaxSupressionImage[h - 1][w - 1].r >= maxThreshold;
						isRemain |= MaxSupressionImage[h - 1][w].r >= maxThreshold;
						isRemain |= MaxSupressionImage[h - 1][w + 1].r >= maxThreshold;
						isRemain |= MaxSupressionImage[h][w - 1].r >= maxThreshold;
						isRemain |= MaxSupressionImage[h][w + 1].r >= maxThreshold;
						isRemain |= MaxSupressionImage[h + 1][w - 1].r >= maxThreshold;
						isRemain |= MaxSupressionImage[h + 1][w].r >= maxThreshold;
						isRemain |= MaxSupressionImage[h + 1][w + 1].r >= maxThreshold;
						ResultImage[h][w] = isRemain ? Palette::White : Palette::Black;
					}
				};

			int32 width = image.width();
			int32 height = image.height();

			// Step1: ガウスフィルタを適用
			for (int w = 0; w < width; w++)
			{
				for (int h = 0; h < height; h++)
				{
					if (w < filterSize || width - filterSize <= w ||
						h < filterSize || height - filterSize <= h)
					{
						GaussImage[h][w] = Palette::Black;
						continue;
					}
					GaussfilterProcess(w, h);
				}
			}

			// Step2: Sobelを計算(縦横)
			for (int w = 0; w < width; w++)
			{
				for (int h = 0; h < height; h++)
				{
					if (w < filterSize || width - filterSize <= w ||
						h < filterSize || height - filterSize <= h)
					{
						SobelHorizontalImage[h][w] = Palette::Black;
						continue;
					}
					SobelHorizontalProcess(w, h);
				}
			}

			for (int w = 0; w < width; w++)
			{
				for (int h = 0; h < height; h++)
				{
					if (w < filterSize || width - filterSize <= w ||
						h < filterSize || height - filterSize <= h)
					{
						SobelVerticalImage[h][w] = Palette::Black;
						continue;
					}
					SobelVerticalProcess(w, h);
				}
			}

			// Step3: 画像の勾配の強度と方向を計算
			for (int w = 0; w < width; w++)
			{
				for (int h = 0; h < height; h++)
				{
					if (w < filterSize || width - filterSize <= w ||
						h < filterSize || height - filterSize <= h)
					{
						GradientImage[h][w] = Palette::Black;
						DirectionImage[h][w] = Palette::Black;
						continue;
					}
					GradientProcess(w, h);
				}
			}

			// Step4: 勾配を見て値を判断していく
			for (int w = 0; w < width; w++)
			{
				for (int h = 0; h < height; h++)
				{
					if (w < filterSize || width - filterSize <= w ||
						h < filterSize || height - filterSize <= h)
					{
						MaxSupressionImage[h][w] = Palette::Black;
						continue;
					}
					NonMaximumSuprresionProcess(w, h);
				}
			}

			// Step5: 閾値判定
			for (int w = 0; w < width; w++)
			{
				for (int h = 0; h < height; h++)
				{
					if (w < filterSize || width - filterSize <= w ||
						h < filterSize || height - filterSize <= h)
					{
						ResultImage[h][w] = Palette::Black;
						continue;
					}
					HysteresisThresholdProcess(w, h);
				}
			}

			return ResultImage;
		};


	Image image = MakeImage();
	const Texture texture{ image };

	while (System::Update())
	{
		texture.draw();
	}
}
