#include "MultivariateGaussianMixture.h"

MultivariateGaussianMixture::MultivariateGaussianMixture(std::shared_ptr<SimpleGrid> grid, int componentNum)
	: FunctionBase(grid)
	, m_componentNum(componentNum)
{
}
