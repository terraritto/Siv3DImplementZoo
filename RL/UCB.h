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

		double maxValue = -10000;
		for (int i = 0; i < m_arm; i++)
		{
			m_realRewards[i] = m_distribution(m_engine);
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
	int Action() const
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

		// 推定を更新
		m_estimates[action] += (reward - m_estimates[action]) / m_actionCounts[action];

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

public:
	int m_arm = 10; // 腕の数
	int m_stepCount = 0; // ステップ回数
	double m_stepSize = 0.1; // ステップのサイズ
	double m_epsilon = 0.0; // Epsilonの確率
	double m_init = 0.0; // 推定の初期値

	int m_bestAction = 0; // 最適なindex

	bool m_isUcb = false;
	double m_ucbParam = 0.0; // UCBの係数

private:
	// Seed関係
	std::random_device m_seedGen;
	std::default_random_engine m_engine = std::default_random_engine(m_seedGen());
	std::normal_distribution<> m_distribution = std::normal_distribution<>(0.0, 1.0);
	Array<double> m_realRewards; // rewardの真値
	Array<double> m_estimates; // rewardの予測
	Array<int> m_actionCounts; // 各腕でactionが行われた回数
};

void UCBSample()
{
	constexpr int runs = 2000; // 10本腕x2000
	constexpr int times = 1000; // 1000回試行

	Bandit bandit, bandit010;


	Array<double> meanRewards; meanRewards.resize(times, 0.0);
	Array<double> meanRewards010; meanRewards010.resize(times, 0.0);


	// Simulate
	bandit.m_isUcb = true;
	bandit.m_ucbParam = 2.0;
	for (int r = 0; r < runs; r++)
	{
		bandit.Reset();
		for (int i = 0; i < times; i++)
		{
			int action = bandit.Action();
			meanRewards[i] += bandit.Step(action);
		}
	}

	bandit010.m_epsilon = 0.10;
	for (int r = 0; r < runs; r++)
	{
		bandit010.Reset();
		for (int i = 0; i < times; i++)
		{
			int action = bandit010.Action();
			meanRewards010[i] += bandit010.Step(action);
		}
	}

	std::for_each(meanRewards.begin(), meanRewards.end(), [runs](double& value) {value /= runs; });
	std::for_each(meanRewards010.begin(), meanRewards010.end(), [runs](double& value) {value /= runs; });

	auto sceneSize = Scene::Size();
	Scene::SetBackground(Palette::White);

	// 描画画像
	const RenderTexture renderTexture{ sceneSize, Palette::White };

	// グラフ描画
	{
		const ScopedRenderTarget2D target{ renderTexture };

		constexpr double rightValue = times;
		constexpr double topValue = 1.6;

		// Rewardの描画
		for (int i = 1; i < meanRewards.size(); i++)
		{
			double prevX = (i - 1) / rightValue * sceneSize.x;
			double currentX = i / rightValue * sceneSize.x;
			double prevY = (1.0 - meanRewards[i - 1] / topValue) * sceneSize.y;
			double currentY = (1.0 - meanRewards[i] / topValue) * sceneSize.y;
			Line{ Vec2{prevX, prevY}, Vec2{currentX, currentY} }.draw(Palette::Red);
		}

		for (int i = 1; i < meanRewards010.size(); i++)
		{
			double prevX = (i - 1) / rightValue * sceneSize.x;
			double currentX = i / rightValue * sceneSize.x;
			double prevY = (1.0 - meanRewards010[i - 1] / topValue) * sceneSize.y;
			double currentY = (1.0 - meanRewards010[i] / topValue) * sceneSize.y;
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
