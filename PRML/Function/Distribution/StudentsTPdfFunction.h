#pragma once
#include "../../FunctionBase.h"
#include <Eigen/Dense>

class StudentsTPdfFunction : public FunctionBase
{
public:
	StudentsTPdfFunction(std::shared_ptr<SimpleGrid> grid, Eigen::MatrixXd mu = Eigen::MatrixXd(), Eigen::MatrixXd tau = Eigen::MatrixXd(), Eigen::MatrixXd dof = Eigen::MatrixXd());

	[[nodiscard]]
	Eigen::MatrixXd GetDof() const { return m_dof; }

	[[nodiscard]]
	Eigen::MatrixXd GetTau() const { return m_tau; }

	void SetMu(Eigen::MatrixXd value) { m_mu = value; }
	void SetTau(Eigen::MatrixXd value) { m_tau = value; }

	void Fit(Eigen::MatrixXd data, double learning_rate = 0.01);

	[[nodiscard]]
	Eigen::MatrixXd Pdf(Eigen::MatrixXd data) const;

protected:
	std::pair<Eigen::MatrixXd, Eigen::MatrixXd> Expectation(Eigen::MatrixXd data);

	void Maximization(Eigen::MatrixXd data, Eigen::MatrixXd eta, Eigen::MatrixXd lneta, double learning_rate);

protected:
	Eigen::MatrixXd m_mu, m_tau, m_dof;

	friend class CategoricalPdfFunction;
};
