#include "UniformPdfFunction.h"

UniformPdfFunction::UniformPdfFunction(std::shared_ptr<SimpleGrid> grid, double low, double high)
	: FunctionBase(grid)
	, m_low(low)
	, m_high(high)
{
}

Eigen::MatrixXd UniformPdfFunction::GetSample(int sample) const
{
	std::random_device seed_gen;
	std::default_random_engine engine(seed_gen());
	std::uniform_real_distribution<> dist(0.0, 1.0);

	Eigen::MatrixXd u01Sample(sample, 1);

	for (int i = 0; i < sample; i++)
	{
		u01Sample(i, 0) = dist(engine);
	}

	u01Sample = u01Sample * (m_high - m_low);
	u01Sample.array() += m_low;

	return u01Sample;
}
