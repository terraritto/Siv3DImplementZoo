#pragma once
#include "Siv3D.hpp"

void RKSmaple()
{
	constexpr int Steps = 10000; // ステップ数
	constexpr double h = 0.001; // 時間幅
	constexpr double g = 9.8; // 重力加速度
	constexpr double L = 0.3; // 振り子の長さ
	constexpr double k = g / L;

	double theta = 0.01; // 初期の振り子角度
	double omega = 0; // 初期の角速度

	// 2段2次のRK(ホイン法)
	auto RK2 = [&]()
		{
			const double a1 = omega;
			const double b1 = -k * Sin(theta);

			const double a2 = omega + h * b1;
			const double b2 = -k * Sin(theta + h * a1);

			theta = theta + (a1 + a2) * h / 2.0;
			omega = omega + (b1 + b2) * h / 2.0;
		};

	// 3段3次のRK
	auto RK3 = [&]()
		{
			const double a1 = omega;
			const double b1 = -k * Sin(theta);

			const double a2 = omega + h * b1 / 2.0;
			const double b2 = -k * Sin(theta + h * a1 / 2.0);

			const double a3 = omega + h * (-b1 + b2 * 2.0);
			const double b3 = -k * Sin(theta + h * (- a1 + a2 * 2.0));

			theta = theta + (a1 + 4.0 * a2 + a3) * h / 6.0;
			omega = omega + (b1 + 4.0 * b2 + b3) * h / 6.0;
		};

	auto RK4 = [&]()
		{
			const double a1 = omega;
			const double b1 = -k * Sin(theta);

			const double a2 = omega + b1 * h / 2.0;
			const double b2 = -k * Sin(theta + a1 * h / 2.0);

			const double a3 = omega + b2 * h / 2.0;
			const double b3 = -k * Sin(theta + a2 * h / 2.0);

			const double a4 = omega + b3 * h;
			const double b4 = -k * Sin(theta + a3 * h);

			theta = theta + (a1 + 2.0 * a2 + 2.0 * a3 + a4) * h / 6.0;
			omega = omega + (b1 + 2.0 * b2 + 2.0 * b3 + b4) * h / 6.0;
		};

	Array<std::tuple<double, double>> thetaPoints;
	Array<std::tuple<double, double>> omegaPoints;

	for (int i = 0; i < Steps; i++)
	{
		thetaPoints.push_back({ h * i, theta });
		omegaPoints.push_back({ theta, omega });
		RK4();
	}

	auto maxThetaPoint = *(std::max_element(std::begin(thetaPoints), std::end(thetaPoints), [](std::tuple<double, double>& a, std::tuple<double, double> b) { return std::get<1>(a) < std::get<1>(b); }));
	auto minThetaPoint = *(std::max_element(std::begin(thetaPoints), std::end(thetaPoints), [](std::tuple<double, double>& a, std::tuple<double, double> b) { return std::get<1>(a) > std::get<1>(b); }));

	auto maxOmegaPoint = *(std::max_element(std::begin(omegaPoints), std::end(omegaPoints), [](std::tuple<double, double>& a, std::tuple<double, double> b) { return std::get<1>(a) < std::get<1>(b); }));
	auto minOmegaPoint = *(std::max_element(std::begin(omegaPoints), std::end(omegaPoints), [](std::tuple<double, double>& a, std::tuple<double, double> b) { return std::get<1>(a) > std::get<1>(b); }));

	auto sceneSize = Scene::Size();
	Scene::SetBackground(Palette::White);

	while (System::Update())
	{
		/*
		// x軸: 時間 y軸: theta
		double maxTime = get<0>(thetaPoints.back());
		for (auto& v : thetaPoints)
		{
			Vec2 p
			(
				std::get<0>(v) / maxTime * sceneSize.x,
				sceneSize.y - sceneSize.y * ((std::get<1>(maxThetaPoint) - std::get<1>(v)) / (std::get<1>(maxThetaPoint) - std::get<1>(minThetaPoint)))
			);
			Circle{ p, 5.0 }.draw(Palette::Blue);
		}
		*/

		// x軸: theta y軸: 角速度
		for (auto& v : omegaPoints)
		{
			Vec2 p
			(
				(std::get<1>(maxThetaPoint) - std::get<0>(v)) / (std::get<1>(maxThetaPoint) - std::get<1>(minThetaPoint))* sceneSize.x,
				sceneSize.y - sceneSize.y * ((std::get<1>(maxOmegaPoint) - std::get<1>(v)) / (std::get<1>(maxOmegaPoint) - std::get<1>(minOmegaPoint)))
			);
			Circle{ p, 5.0 }.draw(Palette::Blue);
		}
	}
}
