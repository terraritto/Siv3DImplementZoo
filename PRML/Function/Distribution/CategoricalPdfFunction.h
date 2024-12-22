#pragma once
#include "../../FunctionBase.h"
#include "DirichletPdfFunction.h"
#include <Eigen/Dense>

class CategoricalPdfFunction : public FunctionBase
{
public:
	CategoricalPdfFunction(std::shared_ptr<SimpleGrid> grid);
	CategoricalPdfFunction(std::shared_ptr<SimpleGrid> grid, std::weak_ptr<DirichletPdfFunction> dirichlet);

	void Fit(Eigen::MatrixXd data);

	[[nodiscard]]
	Eigen::MatrixXd GetMu() const { return m_mu; }

	[[nodiscard]]
	Eigen::MatrixXd GetAlpha() const { return m_dirichlet->GetAlpha(); }

protected:
	virtual double Calculate(const double x) final;

	std::unique_ptr<DirichletPdfFunction> m_dirichlet;
	Eigen::MatrixXd m_mu;
};
