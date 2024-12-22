#pragma once
#include <Siv3D.hpp>
#include <Eigen/Dense>

class LogisticRegression
{
public:
	void fit(Array<Array<double>> xTrain, Array<double> yTrain, int maxIter = 100);

	[[nodiscard]]
	Eigen::MatrixXd classify(Array<Array<double>> xTest, double threshold = 0.5);

	[[nodiscard]]
	virtual Eigen::MatrixXd Proba(Array<Array<double>> xTest);

protected:
	Eigen::MatrixXd Sigmoid(Eigen::MatrixXd a);

protected:
	Eigen::MatrixXd m_w;
};
