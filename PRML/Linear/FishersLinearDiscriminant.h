#pragma once
#include <Siv3D.hpp>
#include <Eigen/Dense>

class FisaherLinearDiscriminant
{
public:
	FisaherLinearDiscriminant();

	void fit(Array<Array<double>> xTrain, Array<double> yTrain);

	[[nodiscard]]
	Eigen::MatrixXd classify(Array<Array<double>> xTest);

protected:
	Eigen::MatrixXd m_w;
	double m_threshold;
};
