#include "ToyLinearFunction.h"

ToyLinearFunction::ToyLinearFunction(std::shared_ptr<SimpleGrid> grid, double mean, double std)
	: FunctionBase(grid)
	, m_randomDevice()
	, m_engine(std::default_random_engine(m_randomDevice()))
	, m_normalDistribution(std::normal_distribution<>(mean, std))
	, m_a(0.5)
	, m_b(-0.3)
{
}

double ToyLinearFunction::Calculate(const double x)
{
	return m_a * x + m_b + m_normalDistribution(m_engine);
}
