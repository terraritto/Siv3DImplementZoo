#pragma once
#include <Siv3D.hpp>
#include <Eigen/Dense>

class RidgeRegression
{
public:
	RidgeRegression(double alpha);

	void fit(Array<Array<double>> xTrain, Array<double> yTrain);

	[[nodiscard]]
	Array<double> Predict(Array<Array<double>> xTest);

private:
	Eigen::MatrixXd m_weight;

	double m_alpha;
};
