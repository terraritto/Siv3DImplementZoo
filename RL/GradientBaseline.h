#pragma once
#include <Siv3D.hpp>

/*
MIT License

Copyright (c) 2019 Shangtong Zhang

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

class Bandit
{
public:
	void Reset()
	{
		m_realRewards.clear(); m_realRewards.resize(m_arm);
		m_estimates.clear(); m_estimates.resize(m_arm);
		m_actionCounts.clear(); m_actionCounts.resize(m_arm);
		m_actionProbs.clear(); m_actionProbs.resize(m_arm);

		double maxValue = -10000;
		for (int i = 0; i < m_arm; i++)
		{
			m_realRewards[i] = m_distribution(m_engine) + m_trueReward;
			m_estimates[i] = m_init;
			m_actionCounts[i] = 0;

			if (maxValue < m_realRewards[i])
			{
				m_bestAction = i;
				maxValue = m_realRewards[i];
			}
		}

		m_stepCount = 0;
	}

	// バンデットでのアクションを行うIndexを返す
	int Action()
	{
		double value = Random(0.0, 1.0);
		if (value < m_epsilon)
		{
			return Random(0, m_arm - 1);
		}

		if (m_isUcb)
		{
			return ActionUCB();
		}

		if (m_isGradient)
		{
			return ActionGradient();
		}

		// 予測の最大値
		double best = *std::max_element(m_estimates.begin(), m_estimates.end());

		// 最大値のものを取り出す
		Array<int> index;
		for (int count = 0; const auto & value : m_estimates)
		{
			if (best == value) { index.push_back(count); }
			count++;
		}

		// 一つ選ぶ
		return index.choice();
	}

	double Step(int action)
	{
		// Rewardの取得
		double reward = m_distribution(m_engine) + m_realRewards[action];

		// ステップを一つ足しておく
		m_stepCount++;
		m_actionCounts[action]++;
		m_averageReward += (reward - m_averageReward) / m_stepCount;

		// 推定を更新
		if (m_isSampleAverage)
		{
			m_estimates[action] += (reward - m_estimates[action]) / m_actionCounts[action];
		}
		else if (m_isGradient)
		{
			// 選択したもののみ1の配列
			Array<double> oneHots;
			oneHots.resize(m_estimates.size(), 0.0);
			oneHots[action] = 1.0;

			// ベースラインの設定
			double baseline = 0.0;
			if (m_isGradientBaseline)
			{
				baseline = m_averageReward;
			}

			// 更新
			for (int i = 0; i < m_estimates.size(); i++)
			{
				m_estimates[i] += m_stepSize * (reward - baseline) * (oneHots[i] - m_actionProbs[i]);
			}
		}
		else
		{
			m_estimates[action] += m_stepSize * (reward - m_estimates[action]);
		}

		return reward;
	}

public:
	int ActionUCB() const
	{
		Array<double> ucbEstimates;
		for (int i = 0; i < m_estimates.size(); i++)
		{
			double ucbParam = m_estimates[i] +
				m_ucbParam * Sqrt(Log(m_stepCount + 1) / (m_actionCounts[i] + 1e-5));
			ucbEstimates.push_back(ucbParam);
		}

		double best = *std::max_element(ucbEstimates.begin(), ucbEstimates.end());
		Array<int> index;
		for (int count = 0; const auto & value : ucbEstimates)
		{
			if (best == value) { index.push_back(count); }
			count++;
		}

		// 一つ選ぶ
		return index.choice();
	}

	int ActionGradient()
	{
		Array<double> expEstimates;
		double expEstimateSum = 0.0;
		for (int i = 0; i < m_estimates.size(); i++)
		{
			double expValue = Exp(m_estimates[i]);
			expEstimateSum += expValue;
			expEstimates.push_back(expValue);
		}

		for (int i = 0; i < m_actionProbs.size(); i++)
		{
			m_actionProbs[i] = expEstimates[i] / expEstimateSum;
		}

		// 一つ選ぶ
		std::discrete_distribution<std::size_t> distribution(m_actionProbs.begin(), m_actionProbs.end());
		return distribution(m_engine);
	}

public:
	int m_arm = 10; // 腕の数
	int m_stepCount = 0; // ステップ回数
	double m_stepSize = 0.1; // ステップのサイズ
	double m_epsilon = 0.0; // Epsilonの確率
	double m_init = 0.0; // 推定の初期値
	double m_trueReward = 0.0; // Rewardに足す下駄

	int m_bestAction = 0; // 最適なindex

	bool m_isUcb = false;
	double m_ucbParam = 0.0; // UCBの係数

	bool m_isSampleAverage = false; // 一番単純な取得を利用するか？

	bool m_isGradient = false;
	bool m_isGradientBaseline = false;

private:
	// Seed関係
	std::random_device m_seedGen;
	std::default_random_engine m_engine = std::default_random_engine(m_seedGen());
	std::normal_distribution<> m_distribution = std::normal_distribution<>(0.0, 1.0);

	// 平均のReward
	double m_averageReward = 0.0;

	// 使用配列
	Array<double> m_realRewards; // rewardの真値
	Array<double> m_estimates; // rewardの予測
	Array<double> m_actionProbs; // Action時の確率
	Array<int> m_actionCounts; // 各腕でactionが行われた回数
};

void GradientLineSample()
{
	constexpr int runs = 2000; // 10本腕x2000
	constexpr int times = 1000; // 1000回試行

	Bandit bandit01, bandit02, bandit03, bandit04;

	Array<double> bestActions01; bestActions01.resize(times, 0.0);
	Array<double> bestActions02; bestActions02.resize(times, 0.0);
	Array<double> bestActions03; bestActions03.resize(times, 0.0);
	Array<double> bestActions04; bestActions04.resize(times, 0.0);

	// Simulate
	bandit01.m_isGradient = true;
	bandit01.m_stepSize = 0.1;
	bandit01.m_isGradientBaseline = true;
	bandit01.m_trueReward = 4.0;
	for (int r = 0; r < runs; r++)
	{
		bandit01.Reset();
		for (int i = 0; i < times; i++)
		{
			int action = bandit01.Action();
			bandit01.Step(action);

			if (action == bandit01.m_bestAction)
			{
				bestActions01[i] += 1.0;
			}
		}
	}

	bandit02.m_isGradient = true;
	bandit02.m_stepSize = 0.1;
	bandit02.m_isGradientBaseline = false;
	bandit02.m_trueReward = 4.0;
	for (int r = 0; r < runs; r++)
	{
		bandit02.Reset();
		for (int i = 0; i < times; i++)
		{
			int action = bandit02.Action();
			bandit02.Step(action);
			if (action == bandit02.m_bestAction)
			{
				bestActions02[i] += 1.0;
			}
		}
	}

	bandit03.m_isGradient = true;
	bandit03.m_stepSize = 0.4;
	bandit03.m_isGradientBaseline = true;
	bandit03.m_trueReward = 4.0;
	for (int r = 0; r < runs; r++)
	{
		bandit03.Reset();
		for (int i = 0; i < times; i++)
		{
			int action = bandit03.Action();
			bandit03.Step(action);
			if (action == bandit03.m_bestAction)
			{
				bestActions03[i] += 1.0;
			}
		}
	}

	bandit04.m_isGradient = true;
	bandit04.m_stepSize = 0.4;
	bandit04.m_isGradientBaseline = false;
	bandit04.m_trueReward = 4.0;
	for (int r = 0; r < runs; r++)
	{
		bandit04.Reset();
		for (int i = 0; i < times; i++)
		{
			int action = bandit04.Action();
			bandit04.Step(action);
			if (action == bandit04.m_bestAction)
			{
				bestActions04[i] += 1.0;
			}
		}
	}

	std::for_each(bestActions01.begin(), bestActions01.end(), [runs](double& value) {value /= runs; });
	std::for_each(bestActions02.begin(), bestActions02.end(), [runs](double& value) {value /= runs; });
	std::for_each(bestActions03.begin(), bestActions03.end(), [runs](double& value) {value /= runs; });
	std::for_each(bestActions04.begin(), bestActions04.end(), [runs](double& value) {value /= runs; });

	auto sceneSize = Scene::Size();
	Scene::SetBackground(Palette::White);

	// 描画画像
	const RenderTexture renderTexture{ sceneSize, Palette::White };

	// グラフ描画
	{
		const ScopedRenderTarget2D target{ renderTexture };

		constexpr double rightValue = times;
		constexpr double topValue = 0.9;

		// Rewardの描画
		for (int i = 1; i < bestActions01.size(); i++)
		{
			double prevX = (i - 1) / rightValue * sceneSize.x;
			double currentX = i / rightValue * sceneSize.x;
			double prevY = (1.0 - bestActions01[i - 1] / topValue) * sceneSize.y;
			double currentY = (1.0 - bestActions01[i] / topValue) * sceneSize.y;
			Line{ Vec2{prevX, prevY}, Vec2{currentX, currentY} }.draw(Palette::Red);
		}

		for (int i = 1; i < bestActions02.size(); i++)
		{
			double prevX = (i - 1) / rightValue * sceneSize.x;
			double currentX = i / rightValue * sceneSize.x;
			double prevY = (1.0 - bestActions02[i - 1] / topValue) * sceneSize.y;
			double currentY = (1.0 - bestActions02[i] / topValue) * sceneSize.y;
			Line{ Vec2{prevX, prevY}, Vec2{currentX, currentY} }.draw(Palette::Blue);
		}

		for (int i = 1; i < bestActions03.size(); i++)
		{
			double prevX = (i - 1) / rightValue * sceneSize.x;
			double currentX = i / rightValue * sceneSize.x;
			double prevY = (1.0 - bestActions03[i - 1] / topValue) * sceneSize.y;
			double currentY = (1.0 - bestActions03[i] / topValue) * sceneSize.y;
			Line{ Vec2{prevX, prevY}, Vec2{currentX, currentY} }.draw(Palette::Purple);
		}

		for (int i = 1; i < bestActions04.size(); i++)
		{
			double prevX = (i - 1) / rightValue * sceneSize.x;
			double currentX = i / rightValue * sceneSize.x;
			double prevY = (1.0 - bestActions04[i - 1] / topValue) * sceneSize.y;
			double currentY = (1.0 - bestActions04[i] / topValue) * sceneSize.y;
			Line{ Vec2{prevX, prevY}, Vec2{currentX, currentY} }.draw(Palette::Green);
		}

		// 1.0のところの線
		double oneHeight = (1.0 - 1.0 / topValue) * sceneSize.y;
		Line{ Vec2{0.0, oneHeight}, Vec2{sceneSize.x, oneHeight} }.draw(Palette::Black);
	}


	while (System::Update())
	{
		renderTexture.draw();
	}

}
