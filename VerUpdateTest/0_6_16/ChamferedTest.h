#pragma once
#include <Siv3D.hpp>

void ChamferedTest()
{
	Scene::SetBackground(ColorF{ 0.6, 0.8, 0.7 });
	const Font font{ FontMethod::MSDF, 48, Typeface::Bold };

	const Rect button1{ 100, 100, 320, 100 };
	const Rect button1_original{ 450, 100, 320, 100 };
	const Rect button2{ 100, 300, 320, 100 };
	const Rect button2_original{ 450, 300, 320, 100 };

	double s = 10.0;
	double tl = 50.0;
	double tr = 50.0;
	double bl = 50.0;
	double br = 50.0;

	while (System::Update())
	{
		button1.chamfered(s).draw(ColorF{ 0.3, 0.6, 0.9 });
		font(U"アイテム").drawAt(44, button1.center(), ColorF{ 1.0 });
		button1_original.draw(ColorF{ 0.3, 0.6, 0.9 });
		font(U"アイテム").drawAt(44, button1_original.center(), ColorF{ 1.0 });

		button2.chamfered(tl, tr, bl, br).draw(ColorF{ 0.3, 0.6, 0.9 });
		font(U"アイテム").drawAt(44, button2.center(), ColorF{ 1.0 });
		button2_original.draw(ColorF{ 0.3, 0.6, 0.9 });
		font(U"アイテム").drawAt(44, button2_original.center(), ColorF{ 1.0 });
	}
}
