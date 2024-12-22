#pragma once
#include <Siv3D.hpp>
#include <Eigen/Dense>
#include "LogisticRegression.h"

class BayesianLogisticRegression : public LogisticRegression
{
public:
	BayesianLogisticRegression(double alpha = 1.0);

	void fit(Array<Array<double>> xTrain, Array<double> yTrain, int maxIter = 100);

	[[nodiscard]]
	Eigen::MatrixXd Proba(Array<Array<double>> xTest) override;

protected:
	Eigen::MatrixXd m_precision;
	Eigen::MatrixXd m_mean;

	double m_alpha;
};
