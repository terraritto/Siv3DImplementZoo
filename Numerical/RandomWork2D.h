#pragma once
#include "Siv3D.hpp"

void RandomWork2D()
{
	Image image(Scene::Size(), Palette::Black);
	DynamicTexture texture(image);


	texture.fill(image);

	constexpr int MAX_NUM = 50000;
	Array<Vector2D<int>> walkProgress;

	// 中心を初期位置
	Vector2D<int> currentPosition = Scene::Size() / 2;
	walkProgress.push_back(currentPosition);

	while (System::Update())
	{
		std::random_device seed_gen;
		std::default_random_engine engine(seed_gen());

		// 位置を更新
		UniformIntDistribution RandEngine(0, 7);
		int RandomDirection = RandEngine(engine);

		switch (RandomDirection)
		{
		case 0: currentPosition += Vector2D<int>(-1, -1); break;
		case 1: currentPosition += Vector2D<int>(0, -1); break;
		case 2: currentPosition += Vector2D<int>(1, -1); break;
		case 3: currentPosition += Vector2D<int>(-1, 0); break;
		case 4: currentPosition += Vector2D<int>(1, 0); break;
		case 5: currentPosition += Vector2D<int>(-1, 1); break;
		case 6: currentPosition += Vector2D<int>(0, 1); break;
		case 7: currentPosition += Vector2D<int>(1, 1); break;
		}

		// 境界条件
		currentPosition.x = Clamp(currentPosition.x, 0, image.width() - 1);
		currentPosition.y = Clamp(currentPosition.y, 0, image.height() - 1);

		// 位置を追加
		walkProgress.push_back(currentPosition);
		if (MAX_NUM < walkProgress.size())
		{
			walkProgress.pop_front();
		}

		// 初期化
		image.fill(Palette::Black);

		// 塗りつぶし
		for (auto& progress : walkProgress)
		{
			image[progress.y][progress.x] = Palette::Red;
		}

		// 更新してdraw
		texture.fill(image);
		texture.draw();
	}
}
