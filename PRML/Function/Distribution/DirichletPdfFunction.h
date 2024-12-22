#pragma once
#include "../../FunctionBase.h"
#include <Eigen/Dense>

class DirichletPdfFunction : public FunctionBase
{
public:
	DirichletPdfFunction(std::shared_ptr<SimpleGrid> grid, Eigen::MatrixXd alpha);

	[[nodiscard]]
	Eigen::MatrixXd GetAlpha() const { return m_alpha; }

protected:
	Eigen::MatrixXd m_alpha;

	friend class CategoricalPdfFunction;
};
