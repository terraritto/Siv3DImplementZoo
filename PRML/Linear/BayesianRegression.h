#pragma once
#include <Siv3D.hpp>
#include <Eigen/Dense>

class BayesianRegression
{
public:
	BayesianRegression(double alpha = 1.0, double beta = 1.0);

	void fit(Array<Array<double>> xTrain, Array<double> yTrain);

	[[nodiscard]]
	Array<double> Predict(Array<Array<double>> xTest, int sample = 1);

	[[nodiscard]]
	Eigen::MatrixXd GetMean() const { return *m_mean; }

	[[nodiscard]]
	Eigen::MatrixXd GetConv() const { return m_conv; }

protected:
	[[nodiscard]]
	bool IsPriorDefined() const;

protected:
	std::shared_ptr<Eigen::MatrixXd> m_precision;
	std::shared_ptr<Eigen::MatrixXd> m_mean;
	Eigen::MatrixXd m_conv;

	double m_alpha, m_beta;
};
