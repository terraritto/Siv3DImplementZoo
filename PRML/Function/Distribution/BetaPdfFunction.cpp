#include "BetaPdfFunction.h"

BetaPdfFunction::BetaPdfFunction(std::shared_ptr<SimpleGrid> grid, double a, double b)
	: FunctionBase(grid)
	, m_a(a)
	, m_b(b)
{
}

double BetaPdfFunction::Calculate(const double x)
{
	return std::tgamma(m_a + m_b) * Math::Pow(x, m_b - 1) * Math::Pow(1 - x, m_a - 1) / std::tgamma(m_a) / std::tgamma(m_b);
}
