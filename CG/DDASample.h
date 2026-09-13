#pragma once
#include <Siv3D.hpp>

void DDASample()
{
	Image image(Scene::Size(), Palette::Black);

	DynamicTexture texture(image);

	std::queue<Vec2> clickQueue;

	auto DDA = [&image](Vec2 p1, Vec2 p2)
		{
			Vec2 d = p2 - p1;
			double step = Abs(d.x) >= Abs(d.y) ? Abs(d.x) : Abs(d.y);

			// 0の場合は完全に1点のみなので省く
			if (step <= 0.0)
			{
				image[static_cast<int>(p1.y)][static_cast<int>(p1.x)] = Palette::White;
				return;
			}

			d /= step;

			int count = 0;
			while (count <= step)
			{
				image[static_cast<int>(p1.y)][static_cast<int>(p1.x)] = Palette::White;
				p1 += d;
				count++;
			}
		};

	while (System::Update())
	{
		texture.draw();
		if (SimpleGUI::Button(U"Make", Vec2::Zero() * 50))
		{
			if (clickQueue.size() != 2) { continue; }

			Vec2 first = clickQueue.front(); clickQueue.pop();
			Vec2 second = clickQueue.front(); clickQueue.pop();

			DDA(first, second);

			texture.fill(image);
		}
		else if (MouseL.up())
		{
			clickQueue.push(Cursor::Pos());

			// 2個を保証
			if (clickQueue.size() == 3)
			{
				clickQueue.pop();
			}
		}
	}
}
