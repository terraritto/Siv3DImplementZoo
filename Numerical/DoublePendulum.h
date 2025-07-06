#pragma once
#include "Siv3D.hpp"

void DoublePendulumSmaple()
{
	constexpr double h = 0.01, h2 = h * 0.5; // 時間幅
	constexpr double g = 9.8; // 重力加速度
	constexpr double L = 0.3; // 振り子の長さ
	constexpr double k = g / L;

	double theta1 = -Math::Pi / 2.0, theta2 = -Math::Pi / 4.0; // 初期の振り子速度
	double omega1 = 0.0, omega2 = 0.0; // 初期の振り子角速度
	double m1 = 1.0, m2 = 1.0;
	double L1 = 1.0, L2 = 1.0;

	auto f1 = [](double omega) { return omega; };
	auto f2 = [](double omega) { return omega; };

	auto g1 = [&](double theta1, double theta2, double omega1, double omega2)
		{
			double n1 = g * (2.0 * m1 + m2) * Sin(theta1);
			double n2 = m2 * g * Sin(theta1 - 2.0 * theta2);
			double n31 = 2.0 * sin(theta1 - theta2) * m2;
			double n32 = omega2 * omega2 * L2 + omega1 * omega1
				* L1 * Cos(theta1 - theta2);
			double n = -n1 - n2 - n31 * n32;
			double d = L1 * (2 * m1 + m2
				- m2 * Cos(2 * theta1 - 2 * theta2));
			return n / d;
		};

	auto g2 = [&](double theta1, double theta2, double omega1, double omega2)
		{
			double n1 = g * (m1 + m2) * Cos(theta1);
			double n2 = L1 * omega1 * omega1 * (m1 + m2);
			double n3 = L2 * omega2 * omega2 * m2 * Cos(theta1 - theta2);
			double n4 = 2 * Sin(theta1 - theta2);
			double n = (n1 + n2 + n3) * n4;
			double d = L2 * (2 * m1 + m2
				- m2 * Cos(2 * theta1 - 2 * theta2));
			return n / d;
		};

	auto RK4 = [&]()
		{
			const double a1 = f1(omega1);
			const double b1 = g1(theta1, theta2, omega1, omega2);
			const double c1 = f2(omega2);
			const double d1 = g2(theta1, theta2, omega1, omega2);

			const double a2 = f1(omega1 + b1 * h2);
			const double b2 = g1(theta1 + a1 * h2, theta2 + c1 * h2,
				omega1 + b1 * h2, omega2 + d1 * h2);
			const double c2 = f2(omega2 + d1 * h2);
			const double d2 = g2(theta1 + a1 * h2, theta2 + c1 * h2,
				omega1 + b1 * h2, omega2 + d1 * h2);

			const double a3 = f1(omega1 + b2 * h2);
			const double b3 = g1(theta1 + a2 * h2, theta2 + c2 * h2,
				omega1 + b2 * h2, omega2 + d2 * h2);
			const double c3 = f2(omega2 + d2 * h2);
			const double d3 = g2(theta1 + a2 * h2, theta2 + c2 * h2,
				omega1 + b2 * h2, omega2 + d2 * h2);

			const double a4 = f1(omega1 + b3 * h2);
			const double b4 = g1(theta1 + a3 * h2, theta2 + c3 * h2,
				omega1 + b3 * h2, omega2 + d3 * h2);
			const double c4 = f2(omega2 + d3 * h2);
			const double d4 = g2(theta1 + a3 * h2, theta2 + c3 * h2,
				omega1 + b3 * h2, omega2 + d3 * h2);

			theta1 = theta1 + (a1 + 2.0 * a2 + 2.0 * a3 + a4) * h / 6.0;
			theta2 = theta2 + (c1 + 2.0 * c2 + 2.0 * c3 + c4) * h / 6.0;
			omega1 = omega1 + (b1 + 2.0 * b2 + 2.0 * b3 + b4) * h / 6.0;
			omega2 = omega2 + (d1 + 2.0 * d2 + 2.0 * d3 + d4) * h / 6.0;
		};

	auto sceneSize = Scene::Size();
	Scene::SetBackground(Palette::White);

	// 描画画像
	const RenderTexture renderTexture{ sceneSize, Palette::White };

	bool isStart = false;
	Timer timer{ 0.001s };

	const Vec2 StartPoint = {sceneSize / 2.0};
	const double BaseLineOffset = 100.0;

	const int drawCounter = 2;
	int i = 0;

	while (System::Update())
	{
		if (isStart && timer.reachedZero())
		{
			// 計算
			RK4();
			timer.restart();

			if (i % drawCounter == 0)
			{
				const ScopedRenderTarget2D target{ renderTexture };
				// 描画
				Vec2 dir1 = Vec2::Down().rotate(theta1);
				Vec2 s1 = StartPoint + dir1 * BaseLineOffset * L1;
				Vec2 dir2 = dir1.rotate(theta2);
				Vec2 s2 = s1 + dir2 * BaseLineOffset * L2;
				Line{ StartPoint, s1 }.draw(Palette::Green);
				Line{ s1, s2 }.draw(Palette::Green);
				Circle{ s1, 10 }.draw(Palette::Red);
				Circle{ s2, 10 }.draw(Palette::Red);
			}

			i++;
		}

		renderTexture.draw();

		if (SimpleGUI::Button(U"Start", Vec2{ 50,50 }))
		{
			isStart = true;
			timer.start();
		}

		if (SimpleGUI::Button(U"Stop", Vec2{ 50,120 }))
		{
			isStart = false;
		}
	}
}
