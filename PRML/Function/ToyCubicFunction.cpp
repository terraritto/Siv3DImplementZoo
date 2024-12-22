#include "ToyCubicFunction.h"

ToyCubicFunction::ToyCubicFunction(std::shared_ptr<SimpleGrid> grid, double mean, double std)
	: FunctionBase(grid)
	, m_randomDevice()
	, m_engine(std::default_random_engine(m_randomDevice()))
	, m_normalDistribution(std::normal_distribution<>(mean, std))
{
}

double ToyCubicFunction::Calculate(const double x)
{
	return x * (x - 0.5) * (x + 0.5) + m_normalDistribution(m_engine);
}
