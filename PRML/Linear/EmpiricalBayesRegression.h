#pragma once
#include "BayesianRegression.h"

class EmpiricalBayesRegression : public BayesianRegression
{
public:
	using BayesianRegression::BayesianRegression;

	void fit(Array<Array<double>> xTrain, Array<double> yTrain, int maxIter = 100);;

	[[nodiscard]]
	double LogEvidence(Array<Array<double>> xTrain, Array<double> yTrain) const;

protected:
	[[nodiscard]]
	double LogPrior(Eigen::MatrixXd w) const;

	[[nodiscard]]
	double LogLikelihood(Eigen::MatrixXd x, Eigen::MatrixXd y, Eigen::MatrixXd w) const;

	[[nodiscard]]
	double LogPosterior(Eigen::MatrixXd x, Eigen::MatrixXd y, Eigen::MatrixXd w) const;
};
