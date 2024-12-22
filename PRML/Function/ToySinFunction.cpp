#include "ToySinFunction.h"

ToySinFunction::ToySinFunction(std::shared_ptr<SimpleGrid> grid, double mean, double std)
	: FunctionBase(grid)
	, m_randomDevice()
	, m_engine(std::default_random_engine(m_randomDevice()))
	, m_normalDistribution(std::normal_distribution<>(mean, std))
{
}

double ToySinFunction::Calculate(const double x)
{
	return Math::Sin(x) + m_normalDistribution(m_engine);
}
