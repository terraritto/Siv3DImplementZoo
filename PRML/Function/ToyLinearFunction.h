#pragma once
#include "../FunctionBase.h"

class ToyLinearFunction : public FunctionBase
{
public:
	ToyLinearFunction(std::shared_ptr<SimpleGrid> grid, double mean = 0.0, double std = 1.0);
	void SetAB(double a, double b) { m_a = a; m_b = b; }

protected:
	virtual double Calculate(const double x) final;
	std::random_device m_randomDevice;
	std::default_random_engine m_engine;
	std::normal_distribution<> m_normalDistribution;

	double m_a, m_b;
};
