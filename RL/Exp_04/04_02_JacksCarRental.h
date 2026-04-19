#pragma once
#include <Siv3D.hpp>

void JacksCarRental()
{
	constexpr int MAX_CARS = 20; // 車の最大数
	constexpr int MAX_MOVE_CARS = 5; // 夜の間に動かす車の数
	constexpr int RENTAL_REQUEST_FIRST_LOCATION = 3; // 最初の位置でのレンタル要望の期待値
	constexpr int RENTAL_REQUEST_SECOND_LOCATION = 4; // 次の位置でのレンタル要望の期待値
	constexpr int RETURN_FIRST_LOCATION = 3; // 最初の位置での車返却の期待値
	constexpr int RETURN_SECOND_LOCATION = 2; // 次の位置での車返却の期待値
	constexpr double DISCOUNT = 0.9; // 割引率
	constexpr double RENTAL_CREDIT = 10.0; // レンタルした際の報酬
	constexpr double MOVE_CAR_COST = 2.0; // 1台の車を動かす際のコスト
	constexpr double POISSON_UPPER_BOUND = 11; // ポアソン分布のnの上界

	// 全てのあり得る行動
	std::vector<int> actions;
	for (int value = -MAX_MOVE_CARS; value <= MAX_MOVE_CARS; value++)
	{
		actions.push_back(value);
	}

	// ポアソン分布の確率
	auto CalculatePoissonProbability = [&](int n, double lambda)
		{
			double factorial = 1.0;
			for (int i = n; i > 0; i--)
			{
				factorial *= i;
			}
			return std::pow(lambda, n) * std::exp(-lambda) / factorial;
		};

	// 期待値計算
	auto ReturnExpected = [&](const Vec2& state, double action, const Array<double>& currentValue)
		{
			double returnValue = 0.0;

			// 車を動かすためのコスト
			returnValue -= MOVE_CAR_COST * Abs(action);

			// 車を動かす
			double firstLocationCars = Min(state.x - action, static_cast<double>(MAX_CARS));
			double secondLocationCars = Min(state.y + action, static_cast<double>(MAX_CARS));

			for (int first = 0; first < POISSON_UPPER_BOUND; first++)
			{
				for (int second = 0; second < POISSON_UPPER_BOUND; second++)
				{
					// 確率
					double firstProb = CalculatePoissonProbability(first, RENTAL_REQUEST_FIRST_LOCATION);
					double secondProb = CalculatePoissonProbability(second, RENTAL_REQUEST_SECOND_LOCATION);
					double probability = firstProb * secondProb;

					double firstLocation = firstLocationCars;
					double secondLocation = secondLocationCars;

					// 実際の車よりも少ない数じゃないと、そもそもリクエスト出来ない
					// なので、Minを取ってリクエスト数を調整
					double validRentalFirstLocation = Min(firstLocation, static_cast<double>(first));
					double validRentalSecondLocation = Min(secondLocation, static_cast<double>(second));
					double totalRentalValidLocation = validRentalFirstLocation + validRentalSecondLocation;

					// レンタルすることによる収益
					// レンタルした後は、そこの車は貸してるので減る
					double reward = totalRentalValidLocation * RENTAL_CREDIT;
					firstLocation -= validRentalFirstLocation;
					secondLocation -= validRentalSecondLocation;

					// 返却時の車
					{
						double returnedFirstLocation = RETURN_FIRST_LOCATION;
						double returnedSecondLocation = RETURN_SECOND_LOCATION;

						// 返却された車の数を加算する
						// ただし、返した結果置ける台数を超えないようにする
						firstLocation += returnedFirstLocation;
						firstLocation = Min(firstLocation, static_cast<double>(MAX_CARS));
						secondLocation += returnedSecondLocation;
						secondLocation = Min(secondLocation, static_cast<double>(MAX_CARS));

						// ベルマン方程式でリターンを計算
						returnValue += probability * (reward + DISCOUNT * currentValue[firstLocation * (MAX_CARS + 1) + secondLocation]);
					}

					// returnもprobに従うように計算してみるver.
					// 激重注意
					/*
					{
						for (int returnedFirst = 0; returnedFirst < POISSON_UPPER_BOUND; returnedFirst++)
						{
							for (int returnedSecond = 0; returnedSecond < POISSON_UPPER_BOUND; returnedSecond++)
							{
								// return時の確率も考慮
								firstProb = CalculatePoissonProbability(returnedFirst, RETURN_FIRST_LOCATION);
								secondProb = CalculatePoissonProbability(returnedSecond, RETURN_SECOND_LOCATION);
								double returnProbability = probability * firstProb * secondProb;

								// 返却された車の数を加算する
								// ただし、返した結果置ける台数を超えないようにする
								int returnedFirstLocation = firstLocation + returnedFirst;
								returnedFirstLocation = Min(returnedFirstLocation, static_cast<int>(MAX_CARS));
								int returnedSecondLocation = secondLocation + returnedSecond;
								returnedSecondLocation = Min(returnedSecondLocation, static_cast<int>(MAX_CARS));

								// ベルマン方程式でリターンを計算
								returnValue += returnProbability * (reward + DISCOUNT * currentValue[returnedFirstLocation * (MAX_CARS + 1) + returnedSecondLocation]);
							}
						}
					}
					*/
				}
			}

			return returnValue;
		};

	Array<Array<double>> results;

	Array<double> values; values.resize((MAX_CARS + 1) * (MAX_CARS + 1), 0.0);
	Array<double> policies; policies = values;
	while (true)
	{
		results.push_back(policies);

		// policy evaluation
		while (true)
		{
			Array<double> oldValues = values;
			for (int i = 0; i < MAX_CARS + 1; i++)
			{
				for (int j = 0; j < MAX_CARS + 1; j++)
				{
					int index = i * (MAX_CARS + 1) + j;
					values[index] = ReturnExpected({i,j}, policies[index], values);
				}
			}

			double maxValue = std::numeric_limits<double>::lowest();
			for (int i = 0; i < values.size(); i++)
			{
				maxValue = Max(maxValue, Abs(oldValues[i] - values[i]));
			}

			// 収束と判断したら終了
			if (maxValue < 1e-4)
			{
				break;
			}
		}

		// policyが安定しているかの判定
		bool isPolicyStable = true;
		for (int i = 0; i < MAX_CARS + 1; i++)
		{
			for (int j = 0; j < MAX_CARS + 1; j++)
			{
				int index = i * (MAX_CARS + 1) + j;
				double oldAction = policies[index];
				Array<double> actionReturns;
				for (const double& action : actions)
				{
					bool isRangeX = ((0 <= action) && (action <= i));
					bool isRangeY = ((-j <= action) && (action <= 0));
					if (isRangeX || isRangeY)
					{
						actionReturns.push_back(ReturnExpected({ i,j }, action, values));
					}
					else
					{
						actionReturns.push_back(std::numeric_limits<double>().lowest());
					}
				}

				int newAction = actions[std::distance(actionReturns.begin(), std::max_element(actionReturns.begin(), actionReturns.end()))];
				policies[index] = newAction;

				if (isPolicyStable && oldAction != newAction)
				{
					// まだ安定してない
					isPolicyStable = false;
				}
			}
		}

		if (isPolicyStable)
		{
			results.push_back(values);
			break;
		}
	}

	int index = 0;

	constexpr Vec2 CENTER_START = { 150,100 };
	constexpr double GRID_SIZE = 18;
	constexpr double GRID_OFFSET = 2;
	constexpr double GRID_OFFSET_SIZE = GRID_SIZE + GRID_OFFSET;
	const Font font{ FontMethod::MSDF, 18, Typeface::Bold };

	constexpr double maxPolicyPrev = 1.0;
	constexpr double minPolicyPrev = -1.0;
	constexpr double maxPolicyLast = 640.0;
	constexpr double minPolicyLast = 440.0;

	while (System::Update())
	{
		auto& value = results[index];

		double maxPolicy = index == results.size() - 1 ? maxPolicyLast : maxPolicyPrev;
		double minPolicy = index == results.size() - 1 ? minPolicyLast : minPolicyPrev;

		for (int i = 0; i < MAX_CARS + 1; i++)
		{
			for (int j = 0; j < MAX_CARS + 1; j++)
			{
				const double policy = (value[i * (MAX_CARS + 1) + j] - minPolicy) / (maxPolicy - minPolicy);
				auto color = Colormap01(policy, s3d::ColormapType::Heat);

				Vec2 center = CENTER_START + Vec2{ j * GRID_OFFSET_SIZE, (MAX_CARS - i) * GRID_OFFSET_SIZE };
				RectF{ Arg::center(center), GRID_SIZE, GRID_SIZE }.draw(color);
			}
		}

		if (SimpleGUI::Button(U"Prev", Vec2{ 10,10 }))
		{
			index--;
			index = std::max(0, index);
		}

		if (SimpleGUI::Button(U"Next", Vec2{ 10,100 }))
		{
			index++;
			index = std::min(static_cast<int>(results.size() - 1), index);
		}
	}
}
