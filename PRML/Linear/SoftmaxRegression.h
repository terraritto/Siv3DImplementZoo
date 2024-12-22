#pragma once
#include <Siv3D.hpp>
#include <Eigen/Dense>

class SoftmaxRegression
{
public:
	SoftmaxRegression();

	void fit(Array<Array<double>> xTrain, Array<double> yTrain, int maxIter = 100, double learingRate = 0.1);

	[[nodiscard]]
	Eigen::MatrixXd classify(Array<Array<double>> xTest);

protected:
	Eigen::MatrixXd Softmax(Eigen::MatrixXd a);

protected:
	Eigen::MatrixXd m_w;
};
