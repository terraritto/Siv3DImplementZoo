#pragma once
#include <Siv3D.hpp>

void NewtonSample()
{
	const ColorF backgroundColor = ColorF{ 0.4, 0.6, 0.8 }.removeSRGBCurve();
	const MSRenderTexture renderTexture{ Scene::Size(), TextureFormat::R8G8B8A8_Unorm_SRGB, HasDepth::Yes };

	// カメラ位置を決めておく
	DebugCamera3D camera{ renderTexture.size(), 30_deg, Vec3{ -2, 0, -2 } };

	auto fx = [](double x) {return x * x * x - 2 * x * x - x + 2; };
	auto fxPrime = [](double x) {return 3 * x * x - 4 * x - 1; };

	auto Newton = [&](double x)
		{
			return x - fx(x) / fxPrime(x);
		};

	Array<Vec2> positions;

	for (int i = 0; i <= 1000; i++)
	{
		double x = Math::Lerp(-2, 3, i / 1000.0);
		double y = fx(x);
		positions.push_back({ x, y });
	}

	constexpr double  initX = 2.0;
	double currentX = 3.0;
	double sliderPos = 3.0;

	constexpr double scale = 0.05;
	int count = 0;

	while (System::Update())
	{
		camera.update(2.0);
		Graphics3D::SetCameraTransform(camera);

		// 3D 描画
		{
			const ScopedRenderTarget3D target{ renderTexture.clear(backgroundColor) };

			for (auto& position : positions)
			{
				Sphere{ {position.x * scale, position.y * scale, 0.0}, 0.01 }.draw(Palette::Red);
			}

			Sphere{ Vec3{-1, fx(-1), 0.0} * scale, 0.02 }.draw(Palette::Green);
			Sphere{ Vec3{1, fx(1), 0.0} * scale, 0.02 }.draw(Palette::Green);
			Sphere{ Vec3{2, fx(2), 0.0} * scale, 0.02 }.draw(Palette::Green);

			Sphere{ Vec3{currentX, fx(currentX), 0.0} *scale, 0.03 }.draw(Palette::Gold);
		}

		// 3D シーンを 2D シーンに描画
		{
			Graphics3D::Flush();
			renderTexture.resolve();
			Shader::LinearToScreen(renderTexture);

			if (SimpleGUI::Button(U"Reset", Vec2{ 100,250 }))
			{
				currentX = initX;
				count = 0;
			}

			if (SimpleGUI::Button(U"Next", Vec2{ 100,300 }))
			{
				currentX = Newton(currentX);
				count++;
			}

			if (SimpleGUI::Button(U"SliderUpdate", Vec2{ 100,350 }))
			{
				currentX = sliderPos;
				count = 0;
			}

			SimpleGUI::Slider(sliderPos, -2, 3, Vec2{ 100, 200 });

			ClearPrint();
			Print << U"CurrentX: " << currentX;
			Print << U"CurrentY: " << fx(currentX);
			Print << U"試行回数: " << count;
		}
	}
}
