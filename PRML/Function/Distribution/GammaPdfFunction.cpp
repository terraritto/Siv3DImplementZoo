#include "GammaPdfFunction.h"

GammaPdfFunction::GammaPdfFunction(std::shared_ptr<SimpleGrid> grid, double a, double b)
	: FunctionBase(grid)
	, m_a(a)
	, m_b(b)
{
}

double GammaPdfFunction::Calculate(const double x)
{
	double Factor1 = Pow(m_b, m_a);
	double Factor2 = Pow(x, m_a - 1);
	double Factor3 = Exp(-m_b * x);
	double Factor4 = tgamma(m_a);
	return Factor1 * Factor2 * Factor3 / Factor4;
}
