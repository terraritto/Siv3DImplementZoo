#pragma once
#include <Siv3D.hpp>

void GamblersProblem()
{
	constexpr int GOAL = 100;
	constexpr double HEAD_PROBABILITY = 0.4;

	// 全ての状態: [0,100]
	Array<int> states;
	for (int value = 1; value <= GOAL; value++)
	{
		states.push_back(value);
	}

	Array<double> values; values.resize(GOAL + 1, 0.0);
	values[GOAL] = 1.0;

	Array<Array<double>> histories;

	while (true)
	{
		Array<double> oldValues = values;
		histories.push_back(oldValues);

		for (auto& state : states)
		{
			// 可能な行動を生成
			Array<int> actions;
			int minAction = Min(state, GOAL - state);
			for (int value = 0; value <= minAction; value++)
			{
				actions.push_back(value);
			}

			// 評価
			Array<double> returns;
			for (auto& action : actions)
			{
				double left = HEAD_PROBABILITY * values[state + action];
				double right = (1.0 - HEAD_PROBABILITY) * values[state - action];
				double result = left + right;
				returns.push_back(result);
			}

			// 良い値で更新
			double newValue = *std::max_element(returns.begin(), returns.end());
			values[state] = newValue;
		}

		double maxValue = std::numeric_limits<double>::lowest();
		for (int i = 0; i < values.size(); i++)
		{
			maxValue = Max(maxValue, Abs(oldValues[i] - values[i]));
		}

		// 収束と判断したら終了
		if (maxValue < 1e-9)
		{
			histories.push_back(values);
			break;
		}
	}

	Array<double> policies; policies.resize(GOAL + 1, 0.0);
	for (auto& state : states)
	{
		if (state == GOAL) { continue; }
		// 可能な行動を生成
		Array<int> actions;
		int minAction = Min(state, GOAL - state);
		for (int value = 0; value <= minAction; value++)
		{
			actions.push_back(value);
		}

		// 評価
		Array<double> returns;
		for (auto& action : actions)
		{
			double left = HEAD_PROBABILITY * values[state + action];
			double right = (1.0 - HEAD_PROBABILITY) * values[state - action];
			double result = left + right;
			returns.push_back(result);
		}

		auto RoundN = [](double value, int n)
			{
				double p = Pow(10, n);
				return Round(value * p) / p;
			};

		returns.pop_front();
		std::for_each(returns.begin(), returns.end(), [&](double& value) { value = RoundN(value, 5); });
		auto maxElement = std::max_element(returns.begin(), returns.end());
		auto distance = std::distance(returns.begin(), maxElement);
		policies[state] = actions[distance + 1];
	}

	bool isSwap = false;

	auto sceneSize = Scene::Size();

	Rect rect{50, 50, sceneSize.x - 50 * 2, sceneSize.y - 50 * 2 };

	auto DrawPolicy = [&]()
		{
			constexpr double MAXVALUE_Y = 50.0;
			int num = policies.size() - 1;
			for (int i = 0; i < policies.size(); i++)
			{
				Vec2 center;
				center.x = static_cast<double>(i) / num * (rect.rightX() - rect.leftX()) + rect.leftX();
				center.y = (1.0 - policies[i] / MAXVALUE_Y) * (rect.bottomY() - rect.topY()) + rect.topY();
				Circle{ center, 3 }.draw(Palette::Red);
			}
		};

	auto DrawSweep = [&]()
		{
			constexpr double MAXVALUE_Y = 1.0;
			for (int i = 0; i < histories.size(); i++)
			{
				auto color = Colormap01(static_cast<double>(i) / (histories.size()-1), s3d::ColormapType::Turbo);
				auto sweep = histories[i];

				Vec2 prev;
				prev.x = rect.leftX();
				prev.y = (1.0 - sweep[0] / MAXVALUE_Y) * (rect.bottomY() - rect.topY()) + rect.topY();

				int num = sweep.size() - 1;
				for (int j = 1; j < sweep.size(); j++)
				{
					Vec2 next;
					next.x = static_cast<double>(j) / num * (rect.rightX() - rect.leftX()) + rect.leftX();
					next.y = (1.0 - sweep[j] / MAXVALUE_Y) * (rect.bottomY() - rect.topY()) + rect.topY();

					Line{ prev, next }.draw(color);
					prev = next;
				}
			}
		};

	Scene::SetBackground(Palette::White);

	while (System::Update())
	{
		if (isSwap)
		{
			DrawPolicy();
		}
		else
		{
			DrawSweep();
		}

		SimpleGUI::CheckBox(isSwap, U"Swap", Vec2{10,10});
	}
}
