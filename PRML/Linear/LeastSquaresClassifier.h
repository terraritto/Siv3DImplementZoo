#pragma once
#include <Siv3D.hpp>
#include <Eigen/Dense>

class LeastSquaresClassifier
{
public:
	LeastSquaresClassifier(Eigen::MatrixXd w = Eigen::MatrixXd());

	void fit(Array<Array<double>> xTrain, Array<double> yTrain);

	[[nodiscard]]
	Eigen::MatrixXd classify(Array<Array<double>> xTest, int sample = 1);

protected:
	Eigen::MatrixXd m_w;
};
