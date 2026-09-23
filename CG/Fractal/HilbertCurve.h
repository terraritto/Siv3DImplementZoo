#pragma once
#include <Siv3D.hpp>

void HilbertCurveSample()
{
	constexpr double LineOffset = 5.0f;

	enum class Direction
	{
		up, down, left, right
	};

	Array<Line> lineList;

	Vec2 currentPos;
	int n = 1;

	std::function<void(int)> L,R,U,D;

	// 方向に沿ってLineを伸ばすだけ
	auto OffsetLine = [&](Direction dir)
	{
		Vec2 temp = currentPos;
		switch (dir)
		{
		case Direction::up:
			currentPos.y -= LineOffset; break;

		case Direction::right:
			currentPos.x += LineOffset; break;

		case Direction::down:
			currentPos.y += LineOffset; break;

		case Direction::left:
			currentPos.x -= LineOffset; break;
		}

		lineList.push_back(Line{ temp, currentPos });
	};

	L = [&](int n)
	{
		if (n <= 0) { return; }
		D(n - 1); OffsetLine(Direction::left);
		L(n - 1); OffsetLine(Direction::down);
		L(n - 1); OffsetLine(Direction::right);
		U(n - 1); 
	};

	U = [&](int n)
	{
		if (n <= 0) { return; }
		R(n - 1); OffsetLine(Direction::up);
		U(n - 1); OffsetLine(Direction::right);
		U(n - 1); OffsetLine(Direction::down);
		L(n - 1);
	};

	R = [&](int n)
	{
		if (n <= 0) { return; }
		U(n - 1); OffsetLine(Direction::right);
		R(n - 1); OffsetLine(Direction::up);
		R(n - 1); OffsetLine(Direction::left);
		D(n - 1);
	};

	D = [&](int n)
	{
		if (n <= 0) { return; }
		L(n - 1); OffsetLine(Direction::down);
		D(n - 1); OffsetLine(Direction::left);
		D(n - 1); OffsetLine(Direction::up);
		R(n - 1);
	};

	Scene::SetBackground(ColorF{ 1.0, 1.0, 1.0 });

	// 開始位置
	constexpr Vec2 StartPos = { 600, 100 };

	// 初期条件
	lineList.clear();
	currentPos = StartPos;
	R(n);

	while (System::Update())
	{
		// 分割が必要なら実行
		if (SimpleGUI::Button(U"分割！！", Vec2{ 10, 10 }))
		{
			lineList.clear();
			currentPos = StartPos;
			n++; L(n);
		}

		// poolしてある線を描画
		for (auto& line : lineList)
		{
			line.draw(Palette::Red);
		}
	}
}
