#pragma once
#include <Siv3D.hpp>

void GregorySharpSample()
{
	class CalcFrame
	{
	public:
		virtual void Step() = 0;

		const Array<double>& GetSamples() const { return m_values; }
		void InitValue(const double& value, const int iterate = 1) { m_iterateValue = value; m_iterate = iterate; }

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

	class Gregory : public CalcFrame
	{
	public:
		Gregory() { InitValue(0.0, 0); }

		void Step() override
		{
			double n = (2.0 * static_cast<double>(m_iterate)) + 1.0;
			m_iterateValue += (m_iterate % 2 == 0 ? 1.0 : -1.0) / n * 4.0;

			// 更新
			m_values.push_back(m_iterateValue);
			m_iterate++;
		}
	};

	class Sharp : public CalcFrame
	{
	public:
		Sharp() { InitValue(0.0, 0); }

		void Step() override
		{
			const double x = 1.0 / Sqrt(3);
			double n = (2.0 * static_cast<double>(m_iterate)) + 1.0;
			m_iterateValue += (m_iterate % 2 == 0 ? 1.0 : -1.0) / n * 6.0 * Pow(x, n);

			// 更新
			m_values.push_back(m_iterateValue);
			m_iterate++;
		}
	};

	// 計算
	Wallis wallis;
	Gregory gregory;
	Sharp sharp;
	for (auto i : step(100))
	{
		wallis.Step();
		gregory.Step();
		sharp.Step();
	}

	auto sceneSize = Scene::Size();
	constexpr double MAX_VALUE_Y = 4.0;
	constexpr double MIN_VALUE_Y = 2.6;

	auto DrawLines = [&](const Array<double>& samples, const ColorF color)
		{
			const auto n = samples.size() - 1;

			{
				for (auto index : step(samples.size() - 1))
				{
					auto prevY = samples[index]; prevY = (MAX_VALUE_Y - prevY) / (MAX_VALUE_Y - MIN_VALUE_Y);
					auto nextY = samples[index + 1]; nextY = (MAX_VALUE_Y - nextY) / (MAX_VALUE_Y - MIN_VALUE_Y);
					prevY *= sceneSize.y; nextY *= sceneSize.y;

					auto prevX = 1.0 - (n - index) / static_cast<double>(n);
					auto nextX = 1.0 - (n - index - 1) / static_cast<double>(n);
					prevX *= sceneSize.x; nextX *= sceneSize.x;

					Line{ {prevX, prevY}, {nextX, nextY} }.draw(color);
				}
			}
		};

	Scene::SetBackground(Palette::Black);

	while (System::Update())
	{
		DrawLines(wallis.GetSamples(), Palette::Red);
		DrawLines(gregory.GetSamples(), Palette::Green);
		DrawLines(sharp.GetSamples(), Palette::Pink);

		auto value = (MAX_VALUE_Y - 3.14) / (MAX_VALUE_Y - MIN_VALUE_Y);
		value *= sceneSize.y;
		Line{ {0, value}, {sceneSize.x, value} }.draw(Palette::Blue);
	}
}
