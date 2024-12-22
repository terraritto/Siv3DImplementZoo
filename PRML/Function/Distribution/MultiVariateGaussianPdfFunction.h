#pragma once
#include "../../FunctionBase.h"
#include <Eigen/Dense>

class MultiVariateGaussianPdfFunction : public FunctionBase
{
public:
	MultiVariateGaussianPdfFunction(std::shared_ptr<SimpleGrid> grid, Eigen::MatrixXd mu = Eigen::MatrixXd(), Eigen::MatrixXd cov = Eigen::MatrixXd());

	[[nodiscard]]
	Eigen::MatrixXd GetMu() const { return m_mu; }

	[[nodiscard]]
	Eigen::MatrixXd GetCov() const { return m_cov; }

	void Fit(Eigen::MatrixXd data);

	[[nodiscard]]
	Eigen::MatrixXd Pdf(Eigen::MatrixXd data) const;

protected:
	Eigen::MatrixXd m_mu, m_cov, m_tau;

	friend class CategoricalPdfFunction;
};
