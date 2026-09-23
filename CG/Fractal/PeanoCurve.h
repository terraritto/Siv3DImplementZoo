#pragma once
#include <Siv3D.hpp>

void PeanoCurveSample()
{
	constexpr double LineOffset = 20.0f;

	enum class Direction
	{
		up, down, left, right
	};

	Array<Line> lineList;

	Vec2 currentPos;
	int n = 0;

	std::function<void(int)> P, Q, R, S;

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

	auto dr = [&](Array<Direction> dirList)
	{
		// 必要な線を構築
		for (auto& dir : dirList)
		{
			Vec2 temp = currentPos;
			switch (dir)
			{
			case Direction::up:
				currentPos.y -= LineOffset * 2.0; break;

			case Direction::right:
				currentPos.x += LineOffset; break;

			case Direction::down:
				currentPos.y += LineOffset * 2.0; break;

			case Direction::left:
				currentPos.x -= LineOffset; break;
			}

			lineList.push_back(Line{ temp, currentPos });
		}
	};

	auto PRoot = [&]() {dr({ Direction::up, Direction::right, Direction::down, Direction::right, Direction::up }); };
	auto QRoot = [&]() {dr({ Direction::up, Direction::left, Direction::down, Direction::left, Direction::up }); };
	auto RRoot = [&]() {dr({ Direction::down, Direction::right, Direction::up, Direction::right, Direction::down }); };
	auto SRoot = [&]() {dr({ Direction::down, Direction::left, Direction::up, Direction::left, Direction::down }); };


	P = [&](int n)
	{
		if (n <= 0) { PRoot(); return; }
		P(n - 1); OffsetLine(Direction::up);
		Q(n - 1); OffsetLine(Direction::up);
		P(n - 1); OffsetLine(Direction::right);
		R(n - 1); OffsetLine(Direction::down);
		S(n - 1); OffsetLine(Direction::down);
		R(n - 1); OffsetLine(Direction::right);
		P(n - 1); OffsetLine(Direction::up);
		Q(n - 1); OffsetLine(Direction::up);
		P(n - 1);
	};

	Q = [&](int n)
	{
		if (n <= 0) { QRoot(); return; }
		Q(n - 1); OffsetLine(Direction::up);
		P(n - 1); OffsetLine(Direction::up);
		Q(n - 1); OffsetLine(Direction::left);
		S(n - 1); OffsetLine(Direction::down);
		R(n - 1); OffsetLine(Direction::down);
		S(n - 1); OffsetLine(Direction::left);
		Q(n - 1); OffsetLine(Direction::up);
		P(n - 1); OffsetLine(Direction::up);
		Q(n - 1);
	};

	R = [&](int n)
	{
		if (n <= 0) { RRoot(); return; }
		R(n - 1); OffsetLine(Direction::down);
		S(n - 1); OffsetLine(Direction::down);
		R(n - 1); OffsetLine(Direction::right);
		P(n - 1); OffsetLine(Direction::up);
		Q(n - 1); OffsetLine(Direction::up);
		P(n - 1); OffsetLine(Direction::right);
		R(n - 1); OffsetLine(Direction::down);
		S(n - 1); OffsetLine(Direction::down);
		R(n - 1);
	};

	S = [&](int n)
	{
		if (n <= 0) { SRoot(); return; }
		S(n - 1); OffsetLine(Direction::down);
		R(n - 1); OffsetLine(Direction::down);
		S(n - 1); OffsetLine(Direction::left);
		Q(n - 1); OffsetLine(Direction::up);
		P(n - 1); OffsetLine(Direction::up);
		Q(n - 1); OffsetLine(Direction::left);
		S(n - 1); OffsetLine(Direction::down);
		R(n - 1); OffsetLine(Direction::down);
		S(n - 1);
	};

	Scene::SetBackground(ColorF{ 1.0, 1.0, 1.0 });

	// 開始位置
	constexpr Vec2 StartPos = { 50, 590 };

	// 初期条件
	lineList.clear();
	currentPos = StartPos;
	P(n);

	while (System::Update())
	{
		// 分割が必要なら実行
		if (SimpleGUI::Button(U"分割！！", Vec2{ 10, 10 }))
		{
			lineList.clear();
			currentPos = StartPos;
			n++; P(n);
		}

		// poolしてある線を描画
		for (auto& line : lineList)
		{
			line.draw(Palette::Red);
		}
	}
}
