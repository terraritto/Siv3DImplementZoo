# include <Siv3D.hpp> // Siv3D v0.6.14

// Trail Sample
/*
void Main()
{
	// 点の基本寿命（秒）
	constexpr double LifeTime = 1.0;

	// 軌跡
	Trail trail{ LifeTime };
	double time = 0.0;

	while (System::Update())
	{
		// 軌跡を更新する
		trail.update();

		// 軌跡に点を追加する
		const double t = Scene::Time();
		time += Scene::DeltaTime();
		const bool isClear = static_cast<int>(time) % 3 > 1;
		const Vec2 pos{ (400 + 200 * Math::Cos(t)), (300 + 200 * Math::Sin(t)) };
		const ColorF color{ 0.3, 0.9, 0.6 };
		const double size = 20.0;
		trail.add(pos, color, size);

		if (isClear)
		{
			trail.clear();
		}

		// 軌跡を描画する
		trail.draw();
	}
}
*/

// MultiPolygon Sample
/*
void Main()
{
	const Polygon polygon01
	{
		Vec2{ 400, 100 }, Vec2{ 600, 300 }, Vec2{ 500, 500 },
	};

	const Polygon polygon02
	{
		Vec2{ 400, 400 }, Vec2{ 300, 500 }, Vec2{ 200, 300 },
	};

	const MultiPolygon multiPolygon{ polygon01, polygon02 };
	const Vec2 centroid = multiPolygon.centroid();

	while (System::Update())
	{
		multiPolygon.draw(Palette::Skyblue);
		multiPolygon.computeConvexHull().outline().draw(Palette::Red);
		Circle{ centroid, 10 }.draw(Palette::Red);
		PutText(U"Area: " + Format(multiPolygon.area()), Arg::topLeft = Vec2{0, 0});
		PutText(U"Perimeter: " + Format(multiPolygon.perimeter()), Arg::topLeft = Vec2{0, 20});
	}
}
*/

// 四角形の回転サンプル
/*
void Main()
{
	Timer timer{ 1s };
	int index = 0;
	s3d::Color colorTable[4] = { Palette::Red, Palette::Blue, Palette::Green, Palette::Yellow };

	timer.start();

	while (System::Update())
	{
		if (timer.reachedZero())
		{
			index++;
			timer.restart();
		}

		int size = 300;
		RectF{ Scene::Width() / 2 - size / 2, Scene::Height() / 2 - size / 2, size, size}.rotate90(index).rotated(index * 10).draw(colorTable[index % 4]);
		
	}
}
*/

// Jsonのそれっぽいやつ
void Main()
{
	// JSON ファイルからデータを読み込む
	const JSON json;
	String text = json.getString(); // 内部でUTF-8 -> UTF-32へ
	std::string utf8TextOld = text.toUTF8(); // UTF-32 -> UTF-8にしてやる

	std::string utf8TextNew = json.getUTF8String(); // UTF-8のまま取得
}
