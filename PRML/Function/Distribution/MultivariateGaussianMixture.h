#pragma once
#include "../../FunctionBase.h"
#include <Eigen/Dense>

class MultivariateGaussianMixture : public FunctionBase
{
public:
	MultivariateGaussianMixture(std::shared_ptr<SimpleGrid> grid, int componentNum);

	[[nodiscard]]
	Eigen::MatrixXd GetCov() const { return m_cov; }

	[[nodiscard]]
	Eigen::MatrixXd GetTau() const { return m_tau; }

	void SetMu(Eigen::MatrixXd value) { m_mu = value; }
	void SetTau(Eigen::MatrixXd value) { m_tau = value; m_cov = 1 / m_tau.array(); }
	void SetCov(Eigen::MatrixXd value) { m_cov = value; m_tau = 1 / m_cov.array(); }

	void Fit(Eigen::MatrixXd data);

	[[nodiscard]]
	Eigen::MatrixXd Pdf(Eigen::MatrixXd data) const;

protected:
	void BayesMu(Eigen::MatrixXd data);

	void BayesTau(Eigen::MatrixXd data);

	void Mu(Eigen::MatrixXd data);

protected:
	Eigen::MatrixXd m_mu, m_cov, m_tau;

	int m_componentNum;
};
