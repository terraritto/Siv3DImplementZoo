#pragma once
#include <Siv3D.hpp>

void GridWorldSample()
{
	// Gridサイズ
	constexpr int WORLD_SIZE = 4;

	using Vec2i = Vector2D<int>;

	// ベルマン方程式用
	constexpr double PROB = 0.25;
	constexpr double DISCOUNT = 1.0;

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

	// 限定の位置
	auto IsTerminal = [&](const Vec2i& state)
		{
			int maxSize = WORLD_SIZE - 1;
			bool isLeftSection = state.x == 0 && state.y == 0;
			bool isRightSection = state.x == maxSize && state.y == maxSize;

			return isLeftSection || isRightSection;
		};

	// 行動
	auto Step = [&](const Vec2i& state, const Vec2i action)
		{
			if (IsTerminal(state))
			{
				return std::tuple{ state, 0 };
			}

			auto nextState = state + action;
			if (nextState.x < 0 || WORLD_SIZE <= nextState.x ||
				nextState.y < 0 || WORLD_SIZE <= nextState.y)
			{
				nextState = state; // 範囲外には移動しないように
			}

			// 基本的に行動は-1の評価
			return std::tuple{ nextState, -1 };
		};

	Array<double> worlds; worlds.resize(WORLD_SIZE * WORLD_SIZE, 0.0);

	// 収束するまで処理を行う
	while (true)
	{
		Array<double> oldValues = worlds;

		for (int i = 0; i < WORLD_SIZE; i++)
		{
			for (int j = 0; j < WORLD_SIZE; j++)
			{
				double value = 0.0;
				for (const auto& action : actions)
				{
					auto [nextState, reward] = Step(Vec2i{ i,j }, GetMoveDirection(action));
					// ベルマン方程式
					value += PROB * (reward + DISCOUNT * worlds[nextState.x * WORLD_SIZE + nextState.y]);
				}

				worlds[i * WORLD_SIZE + j] = value;
			}
		}

		double sum = 0.0;
		for (int i = 0; i < worlds.size(); i++)
		{
			sum += Abs(worlds[i] - oldValues[i]);
		}
		// 収束してるなら終了
		if (sum < 1e-4) { break; }
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
				font(ThousandsSeparate(worlds[i * WORLD_SIZE + j], 1)).draw(Arg::center(center), Palette::Black);
			}
		}
	}
}
