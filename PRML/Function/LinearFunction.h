#pragma once
#include "../FunctionBase.h"

class LinearFunction : public FunctionBase
{
public:
	using FunctionBase::FunctionBase;

	void SetAB(double a, double b) { m_a = a; m_b = b; }

protected:
	virtual double Calculate(const double x) final;

	double m_a = 0.5, m_b = -0.3;
};
