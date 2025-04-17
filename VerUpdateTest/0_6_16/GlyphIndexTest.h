#pragma once
#include <Siv3D.hpp>

void GlyphIndexTest()
{
	Scene::SetBackground(ColorF{ 0.6, 0.8, 0.7 });

	const Font font(40);

	while (System::Update())
	{
		Vec2 pos(20, 20);
		for (int i = 0; i < 20; i++)
		{
			auto glyph = font.getGlyphByGlyphIndex(i + 10);
			glyph.texture.draw(pos, Palette::Black);
			pos.x += 40;
		}
	}
}
