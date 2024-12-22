#pragma once
#include "../FunctionBase.h"

class ToySinFunction : public FunctionBase
{
public:
	ToySinFunction(std::shared_ptr<SimpleGrid> grid, double mean = 0.0, double std = 1.0);

protected:
	virtual double Calculate(const double x) final;
	std::random_device m_randomDevice;
	std::default_random_engine m_engine;
	std::normal_distribution<> m_normalDistribution;
};
