#pragma once
#include <Siv3D.hpp>

void DefaultTextureCreator()
{
	const Font font{ FontMethod::MSDF, 24 };

	Array<String> colorOptions = { U"R", U"G", U"B" };
	size_t colorIndex = 0;

	TextEditState fileNameEditState;

	ColorF targetColor = Palette::Black;

	int textureIndex = 8;

	while (System::Update())
	{
		// カラー用ラジオボタン
		SimpleGUI::RadioButtons(colorIndex, colorOptions, Vec2{ 10, 10 });

		// カラー表示
		font(U"R:" + Format(targetColor.r)).draw(Vec2{10, 125});
		font(U"G:" + Format(targetColor.g)).draw(Vec2{75, 125});
		font(U"B:" + Format(targetColor.b)).draw(Vec2{140, 125});

		// RGB選択
		double* color = nullptr;
		switch (colorIndex)
		{
		case 0: color = &targetColor.r; break;
		case 1: color = &targetColor.g; break;
		case 2: color = &targetColor.b; break;
		default:
			break;
		}

		// 数値変更ボタン
		if (color)
		{
			if (SimpleGUI::Button(U"Zero", Vec2{ 10, 160 }, 50))
			{
				*color = 0.0;
			}

			if (SimpleGUI::Button(U"Half", Vec2{ 60, 160 }, 50))
			{
				*color = 0.5;
			}

			if (SimpleGUI::Button(U"One", Vec2{ 110, 160 }, 50))
			{
				*color = 1.0;
			}
		}

		// Textureのサイズ用
		if (SimpleGUI::Button(U"<", Vec2{ 10, 200 }, 20))
		{
			textureIndex--;
			textureIndex = Max(textureIndex, 0); // 2^0=1
		}

		if (SimpleGUI::Button(U">", Vec2{ 30, 200 }, 20))
		{
			textureIndex++;
			textureIndex = Min(textureIndex, 13); // 2^13=8192
		}

		uint32 TextureSize = 1 << textureIndex;
		font(U"Size:" + Format(TextureSize)+U"x"+Format(TextureSize)).draw(Vec2{60, 200});

		// ファイル名用表示
		SimpleGUI::TextBox(fileNameEditState, Vec2{ 10, 240 });
		if (SimpleGUI::Button(U"Create", Vec2{ 220, 240 }, 70))
		{
			if (!fileNameEditState.text.isEmpty())
			{

				// 単色のRTを用意
				RenderTexture result{TextureSize, TextureSize, targetColor};

				// 画像にして保存
				Image image{TextureSize, TextureSize};
				result.readAsImage(image);
				image.savePNG(fileNameEditState.text + U".png");
			}
		}

		// 色の表示
		Rect{ 75, 10, 100, 100 }.draw(targetColor);
	}
}
