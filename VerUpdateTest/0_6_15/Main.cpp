# include <Siv3D.hpp> // Siv3D v0.6.15

void Main()
{
	Scene::SetBackground(ColorF{ 0.6, 0.8, 0.7 });

	const Font font{ FontMethod::MSDF, 48, Typeface::Bold };
	const String text = U"The quick brown fox jumps over the lazy dog.";

	while (System::Update())
	{
		const Rect rect = Rect::FromPoints(Point{ 50, 20 }, Cursor::Pos());

		rect.draw();

		if (not font(text).draw(60, rect.stretched(-10), ColorF{ 0.0, 1.0 }))
		{
			rect.drawFrame(0, 5, ColorF{ 0.11 });
		}

		if (not font(text).fits(60, rect.stretched(-10)))
		{
			rect.drawFrame(5, 0, ColorF{ 1.0,0.0,0.0 });
		}
	}
}

/*
void Main()
{
	Scene::SetBackground(ColorF{ 0.6, 0.8, 0.7 });

	constexpr Circle drag1Circle{ 200, 200, 50 };
	constexpr Circle drag2Circle{ 400, 200, 50 };

	while (System::Update())
	{
		drag1Circle.draw();

		if (drag1Circle.leftClicked())
		{
			Platform::Windows::DragDrop::MakeDragDrop(U"example/windmill.png");
		}

		drag2Circle.draw();

		if (drag2Circle.leftClicked())
		{
			const Array<FilePath> paths = { U"example/windmill.png", U"example/siv3d-kun.png", U"example/ini/config.ini" };
			Platform::Windows::DragDrop::MakeDragDrop(paths);
		}
	}
}
*/

/*
void Main()
{
	Scene::SetBackground(ColorF{ 0.6, 0.8, 0.7 });

	auto zero = Vec2::Zero();
	auto scalar = Vec2{ 1.5, 2.1 };

	while (System::Update())
	{
		ClearPrint();

		// normalizeは0ベクトルなら0を返す(1ではない)
		Print << U"normalized by zero: " << zero.normalized();
		Print << U"normalized by scalar: " << scalar.normalized();

		// zeroの場合以外は正規化、zeroの場合は入れた数値を返す
		Print << U"normalized by zero: " << zero.normalized_or(scalar);
		Print << U"normalized by scalar: " << scalar.normalized_or(zero);
	}
}
*/

/*
// lerpのサンプル
void Main()
{
	Scene::SetBackground(ColorF{ 0.6, 0.8, 0.7 });

	Window::Resize(1280, 720);

	// 2D カメラ
	Camera2D camera{ Vec2{ 0, 0 }, 1.0, CameraControl::None_ };

	Circular from(100, 0);
	Circular to = from;
	Circular pos = from;

	Circle c1{ pos, 30 };

	Stopwatch stopwatch{ StartImmediately::Yes };

	while (System::Update())
	{
		const double t = Min(stopwatch.sF(), 1.0);
		pos = from.lerp(to, t);

		c1.setCenter(pos);

		// クリックした座標へ移動
		if (MouseL.down())
		{
			from = pos;
			to = Circular{ Cursor::PosF() - Scene::Center() };
			stopwatch.restart();
		}

		// 2D カメラを更新
		camera.update();
		{
			// 2D カメラの設定から Transformer2D を作成する
			const auto t = camera.createTransformer();

			c1.draw(Palette::Pink);

		}

		// 2D カメラ操作の UI を表示する
		camera.draw(Palette::Orange);
	}
}
*/

/*
void Main()
{
	// 画像ファイルから Image を作成
	const Image image{ U"example/windmill.png" };

	// Image から　Texture を作成
	const Texture texture{ image };

	while (System::Update())
	{
		ClearPrint();
		Point ex1 = image.size().xy() - Point::One() * 100;
		Point ex2 = image.size().xy() - Point::One();
		Point ex3 = image.size().xy();
		Point ex4 = image.size().xy() + Point::One() * 100;

		Print << U"image Size: " << image.size();
		Print << ex1 << U" is inbound?: " << image.inBounds(ex1);
		Print << ex2 << U" is inbound?: " << image.inBounds(ex2);
		Print << ex3 << U" is inbound?: " << image.inBounds(ex3);
		Print << ex4 << U" is inbound?: " << image.inBounds(ex4);

		texture.draw(200,200);
	}
}
*/

/*
// IsRunning系のサンプル
void Main()
{
	Scene::SetBackground(ColorF{ 0.6, 0.8, 0.7 });

	while (System::Update())
	{
		ClearPrint();
		Print << U"VisualStudio: " << System::IsRunningInVisualStudio();
		Print << U"XCode: " << System::IsRunningInXcode();
	}
}
*/

/*
// getGrabbedPosのサンプル
void Main()
{
	Scene::SetBackground(ColorF{ 0.6, 0.8, 0.7 });

	// 2D カメラ
	Camera2D camera{ Vec2{ 0, 0 }, 1.0 };

	while (System::Update())
	{
		// 2D カメラを更新
		camera.update();
	
		ClearPrint();
		Print << U"Right Click: " << MouseR.pressed();
		Print << U"Grabbed Pos: " << camera.getGrabbedPos();

		// 2D カメラ操作の UI を表示
		camera.draw(Palette::Orange);
	}
}
*/
