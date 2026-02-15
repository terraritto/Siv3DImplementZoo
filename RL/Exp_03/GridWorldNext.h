#pragma once
#include <Siv3D.hpp>

void GridWorldNextSample()
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
			case Direction::LEFT:	return Vec2i{ -1,0 };
			case Direction::UP:		return Vec2i{ 0, -1 };
			case Direction::DOWN:	return Vec2i{ 0,1 };
			case Direction::RIGHT:	return Vec2i{ 1,0 };
			}
			return Vec2i{ 0,1 };
		};
	auto GetMoveFigs = [](const Direction dir)
		{
			switch (dir)
			{
			case Direction::LEFT:	return U"←";
			case Direction::UP:		return U"↑";
			case Direction::DOWN:	return U"↓";
			case Direction::RIGHT:	return U"→";
			}
			return U"←";
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
				Array<double> values;
				for (const auto& action : actions)
				{
					auto [nextState, reward] = Step(Vec2i{ i,j }, GetMoveDirection(action));
					// ベルマン方程式
					values.push_back(reward + DISCOUNT * worlds[nextState.x * WORLD_SIZE + nextState.y]);
				}

				newValues[i * WORLD_SIZE + j] = *std::max_element(values.begin(), values.end());
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
		{
			// 数値を表示するだけ
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

		{
			// 方向を付与
			for (int i = 0; i < WORLD_SIZE; i++)
			{
				for (int j = 0; j < WORLD_SIZE; j++)
				{
					// 周囲の数値を計算
					Array<std::tuple<double, Direction>> nextValues;
					for (const auto& action : actions)
					{
						auto [nextState, reward] = Step(Vec2i{ i,j }, GetMoveDirection(action));
						if (reward == -1) { continue; } // 範囲外を省いておく
						nextValues.push_back({ worlds[nextState.x * WORLD_SIZE + nextState.y], action });
					}

					// 最大値を取る
					std::tuple<double, Direction> maxValue = *std::max_element(
						nextValues.begin(), nextValues.end(),
						[](const std::tuple<double, Direction>& a, const std::tuple<double, Direction>& b) {
							return std::get<0>(a) < std::get<0>(b);
						});

					// 最大値に近い方向を取得
					String arrow;
					for (auto& value : nextValues)
					{
						if (std::get<0>(maxValue) - std::get<0>(value) < 1e-16)
						{
							arrow += GetMoveFigs(std::get<1>(value));
						}
					}

					// 描画
					Vec2 center = CENTER_START + Vec2{ i * GRID_OFFSET_SIZE, j * GRID_OFFSET_SIZE + 20 };
					font(arrow).draw(Arg::center(center), Palette::Black);
				}
			}
		}

	}
}
