#include "ToyNormalFunction.h"

ToyNormalFunction::ToyNormalFunction(std::shared_ptr<SimpleGrid> grid, double offset, double mean, double std)
	: FunctionBase(grid)
	, m_offset(offset)
	, m_randomDevice()
	, m_engine(std::default_random_engine(m_randomDevice()))
	, m_normalDistribution(std::normal_distribution<>(mean, std))
{
}

double ToyNormalFunction::Calculate(const double)
{
	return m_normalDistribution(m_engine) + m_offset;
}
