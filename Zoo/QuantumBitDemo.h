#pragma once
#include <Siv3D.hpp>

void QuantumBitDemo()
{
	Window::Resize(1280, 720);

	const ColorF backgroundColor = ColorF{ 0.4, 0.6, 0.8 }.removeSRGBCurve();
	const MSRenderTexture renderTexture{ Scene::Size(), TextureFormat::R8G8B8A8_Unorm_SRGB, HasDepth::Yes };
	DebugCamera3D camera{ renderTexture.size(), 30_deg, Vec3{ -1.0, 1.9, -1.0 } };

	constexpr int CELL_NUM = 30;
	constexpr double CELL_SIZE = 0.01;
	constexpr double CELL_OFFSET = 0.01;
	Array<std::pair<Box, ColorF>> cells;
	for (int x = 0; x < CELL_NUM; x++)
	{
		for (int y = 0; y < CELL_NUM; y++)
		{
			for (int z = 0; z < CELL_NUM; z++)
			{
				double offset = CELL_SIZE + CELL_OFFSET;
				ColorF color = ColorF{ Vec3{x,z,y} / (CELL_NUM - 1), 1.0f };
				cells.push_back({ Box{ Vec3{x,y,z} * offset, CELL_SIZE}, color });
			}
		}
	}

	constexpr Vec3 center = (Vec3::One() * (CELL_NUM - 1) * (CELL_SIZE + CELL_OFFSET)) / 2.0;
	const double speed = 0.25f;

	// for ui
	Vec3 probability = Vec3::One();
	constexpr double SLIDER_LABEL_WIDTH = 65.0;
	constexpr double SLIDER_WIDTH = 150.0;
	constexpr Vec2 DEFAULT_POS = { 10,10 };
	constexpr Vec2 DEFAULT_OFFSET = { 0,60 };

	// for display result
	Timer timer{ Duration(10.0) };
	Vec3 result = Vec3::One();

	while (System::Update())
	{
		camera.update(2.0);
		Graphics3D::SetCameraTransform(camera);

		{
			const ScopedRenderTarget3D target{ renderTexture.clear(backgroundColor) };

			double angle = speed * Scene::Time();
			for (auto& cell : cells)
			{
				Box box = cell.first;
				Vec3 boxCenter = box.center;

				// translate origin -> rotate -> translate default
				box.center -= center;
				boxCenter.x = box.center.x * Cos(angle) - box.center.z * Sin(angle);
				boxCenter.z = box.center.x * Sin(angle) + box.center.z * Cos(angle);
				boxCenter += center;

				box.center = boxCenter;

				box.draw(cell.second);
			}

			if (timer.isRunning())
			{
				Vec3 boxPoint = result * (CELL_NUM - 1) * (CELL_SIZE + CELL_OFFSET);

				Vec3 boxCenter = boxPoint;
				boxPoint -= center;
				boxCenter.x = boxPoint.x * Cos(angle) - boxPoint.z * Sin(angle);
				boxCenter.z = boxPoint.x * Sin(angle) + boxPoint.z * Cos(angle);
				boxCenter += center;

				Box{ boxCenter, CELL_SIZE * 4.0 }.draw();
			}
		}

		{
			Graphics3D::Flush();
			renderTexture.resolve();
			Shader::LinearToScreen(renderTexture);

			SimpleGUI::Slider(U"R: " + ThousandsSeparate(probability.x * 100.0, 1), probability.x, DEFAULT_POS, SLIDER_LABEL_WIDTH, SLIDER_WIDTH);
			SimpleGUI::Slider(U"G: " + ThousandsSeparate(probability.z * 100.0, 1), probability.z, DEFAULT_POS + DEFAULT_OFFSET, SLIDER_LABEL_WIDTH, SLIDER_WIDTH);
			SimpleGUI::Slider(U"B: " + ThousandsSeparate(probability.y * 100.0, 1), probability.y, DEFAULT_POS + DEFAULT_OFFSET * 2, SLIDER_LABEL_WIDTH, SLIDER_WIDTH);

			if (SimpleGUI::Button(U"Measure!!", DEFAULT_POS + DEFAULT_OFFSET * 3))
			{
				result.x = Random(0.0, 1.0) < probability.x ? 1.0 : 0.0;
				result.y = Random(0.0, 1.0) < probability.y ? 1.0 : 0.0;
				result.z = Random(0.0, 1.0) < probability.z ? 1.0 : 0.0;
				timer.restart();
			}
		}
	}
}
