#pragma once
#include <Siv3D.hpp>

void GridWorldSample()
{
	// Gridサイズ
	constexpr int WORLD_SIZE = 5;

	using Vec2i = Vector2D<int>;

	// Aの位置関係
	constexpr Vec2i A_POS = { 1,0 };
	constexpr Vec2i A_PRIME_POS = { 1,4 };

	// Bの位置関係
	constexpr Vec2i B_POS = { 3,0 };
	constexpr Vec2i B_PRIME_POS = { 3,2 };

	// ベルマン方程式用
	constexpr double DISCOUNT = 0.9;
	constexpr double PROB = 0.25;

	// 移動方向
	enum class Direction { LEFT, UP, RIGHT, DOWN };
	Array<Direction> actions = { Direction::LEFT, Direction::UP, Direction::RIGHT, Direction::DOWN };
	auto GetMoveDirection = [](const Direction dir)
		{
			switch (dir)
			{
			case Direction::LEFT:	return Vec2i{ 0,-1 };
			case Direction::UP:		return Vec2i{ -1, 0 };
			case Direction::DOWN:	return Vec2i{ 1,0 };
			case Direction::RIGHT:	return Vec2i{ 0,1 };
			}
			return Vec2i{ 0,1 };
		};

	// 行動
	auto Step = [&](const Vec2i& state, const Vec2i action)
		{
			if (state == A_POS) { return std::tuple{ A_PRIME_POS, 10.0 }; }
			if (state == B_POS) { return std::tuple{ B_PRIME_POS, 5.0 }; }

			auto nextState = state + action;
			double reward = 0.0;
			if (nextState.x < 0 || WORLD_SIZE <= nextState.x ||
				nextState.y < 0 || WORLD_SIZE <= nextState.y)
			{
				// 範囲外は-1の報酬
				reward--;
				nextState = state; // 範囲外には移動しないように
			}
			return std::tuple{ nextState, reward };
		};

	Array<double> worlds; worlds.resize(WORLD_SIZE * WORLD_SIZE, 0.0);

	// 収束するまで処理を行う
	while (true)
	{
		Array<double> newValues; newValues.resize(WORLD_SIZE * WORLD_SIZE, 0.0);

		for (int i = 0; i < WORLD_SIZE; i++)
		{
			for (int j = 0; j < WORLD_SIZE; j++)
			{
				for (const auto& action : actions)
				{
					auto [nextState, reward] = Step(Vec2i{ i,j }, GetMoveDirection(action));
					// ベルマン方程式
					newValues[i * WORLD_SIZE + j] += PROB * (reward + DISCOUNT * worlds[nextState.x * WORLD_SIZE + nextState.y]);
				}
			}
		}

		double sum = 0.0;
		for (int i = 0; i < worlds.size(); i++)
		{
			sum += Abs(worlds[i] - newValues[i]);
		}
		// 収束してるなら終了
		if (sum < 1e-4) { break; }

		// 現在のものに更新
		worlds = newValues;
	}

	constexpr Vec2 CENTER_START = { 150,100 };
	constexpr double GRID_SIZE = 90;
	constexpr double GRID_OFFSET = 10;
	constexpr double GRID_OFFSET_SIZE = GRID_SIZE + GRID_OFFSET;
	const Font font{ FontMethod::MSDF, 18, Typeface::Bold };

	while (System::Update())
	{
		for (int i = 0; i < WORLD_SIZE; i++)
		{
			for (int j = 0; j < WORLD_SIZE; j++)
			{
				Vec2 center = CENTER_START + Vec2{ i * GRID_OFFSET_SIZE, j * GRID_OFFSET_SIZE };
				RectF{ Arg::center(center), GRID_SIZE, GRID_SIZE }.draw();
				font(Format(worlds[i * WORLD_SIZE + j])).draw(Arg::center(center), Palette::Black);
			}
		}
	}
}
