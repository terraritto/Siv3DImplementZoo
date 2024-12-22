#pragma once
#include "../../FunctionBase.h"
#include "GammaPdfFunction.h"
#include <Eigen/Dense>

class GaussianPdfFunction : public FunctionBase
{
public:
	GaussianPdfFunction() = default;
	GaussianPdfFunction(std::shared_ptr<SimpleGrid> grid, Eigen::MatrixXd mu = Eigen::MatrixXd(), Eigen::MatrixXd cov = Eigen::MatrixXd());
	GaussianPdfFunction(std::shared_ptr<SimpleGrid> grid, std::weak_ptr<GaussianPdfFunction> mu, Eigen::MatrixXd cov = Eigen::MatrixXd());
	GaussianPdfFunction(std::shared_ptr<SimpleGrid> grid, Eigen::MatrixXd mu, std::weak_ptr<GammaPdfFunction> tau);

	[[nodiscard]]
	std::weak_ptr<GaussianPdfFunction> GetMu() const { return m_muGauss; }

	[[nodiscard]]
	Eigen::MatrixXd GetMuBase() const { return m_mu; }

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

	std::weak_ptr<GaussianPdfFunction> m_muGauss;
	std::weak_ptr<GammaPdfFunction> m_tauGamma;

	friend class CategoricalPdfFunction;
};
