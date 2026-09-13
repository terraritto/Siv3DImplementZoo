#pragma once
#include <Siv3D.hpp>

void WallisSample()
{
	class CalcFrame
	{
	public:
		virtual void Step() = 0;

		const Array<double> GetSamples() const { return m_values; }
		void InitValue(const double& value, const int iterate = 1) { m_iterateValue = value; m_iterate = 1; }

	protected:
		Array<double> m_values;
		double m_iterateValue; int m_iterate;
	};

	class Wallis : public CalcFrame
	{
	public:
		Wallis() { InitValue(2.0); }

		void Step() override
		{
			double n = (2.0 * static_cast<double>(m_iterate));
			n *= n;
			m_iterateValue *= n / (n - 1.0);

			// 更新
			m_values.push_back(m_iterateValue);
			m_iterate++;
		}
	};

	// 計算
	Wallis wallis;
	for (auto i : step(100)) { wallis.Step(); }

	auto sceneSize = Scene::Size();
	constexpr double MAX_VALUE_Y = 4.0;
	constexpr double MIN_VALUE_Y = 2.6;

	while (System::Update())
	{
		const auto samples = wallis.GetSamples();
		const auto n = samples.size() - 1;

		for (auto index : step(samples.size() - 1))
		{
			auto prevY = samples[index]; prevY = (MAX_VALUE_Y - prevY) / (MAX_VALUE_Y - MIN_VALUE_Y);
			auto nextY = samples[index + 1]; nextY = (MAX_VALUE_Y - nextY) / (MAX_VALUE_Y - MIN_VALUE_Y);
			prevY *= sceneSize.y; nextY *= sceneSize.y;

			auto prevX = 1.0 - (n - index) / static_cast<double>(n);
			auto nextX = 1.0 - (n - index - 1) / static_cast<double>(n);
			prevX *= sceneSize.x; nextX *= sceneSize.x;

			Line{ {prevX, prevY}, {nextX, nextY} }.draw(Palette::Red);
		}

		auto value = (MAX_VALUE_Y - 3.14) / (MAX_VALUE_Y - MIN_VALUE_Y);
		value *= sceneSize.y;
		Line{ {0, value}, {sceneSize.x, value} }.draw(Palette::Blue);
	}
}
