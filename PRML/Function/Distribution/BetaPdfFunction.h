#pragma once
#include "../../FunctionBase.h"

class BetaPdfFunction : public FunctionBase
{
public:
	BetaPdfFunction(std::shared_ptr<SimpleGrid> grid, double a = 1.0, double b = 1.0);

protected:
	virtual double Calculate(const double x) final;

	double m_a, m_b;

	friend class BernoulliPdfFunction;
};
