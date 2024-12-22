#pragma once
#include "../../FunctionBase.h"

class GammaPdfFunction : public FunctionBase
{
public:
	GammaPdfFunction(std::shared_ptr<SimpleGrid> grid, double a = 1.0, double b = 1.0);

	[[nodiscard]]
	double GetA() const { return m_a; }

	[[nodiscard]]
	double GetB() const { return m_b; }

	void SetA(double a) { m_a = a; }
	void SetB(double b) { m_b = b; }

protected:
	virtual double Calculate(const double x) final;

	double m_a, m_b;

	friend class BernoulliPdfFunction;
};
