#pragma once
#include "../../FunctionBase.h"
#include <Eigen/Dense>

class UniformPdfFunction : public FunctionBase
{
public:
	UniformPdfFunction(std::shared_ptr<SimpleGrid> grid, double low = 0.0, double high = 0.0);

	[[nodiscard]]
	Eigen::MatrixXd GetSample(int sample) const;

protected:
	double m_low, m_high;

	friend class CategoricalPdfFunction;
};
