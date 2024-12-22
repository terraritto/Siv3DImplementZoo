#include "BernoulliPdfFunction.h"

BernoulliPdfFunction::BernoulliPdfFunction(std::shared_ptr<SimpleGrid> grid, double mu)
	: FunctionBase(grid)
	, m_mu(mu)
{
}

BernoulliPdfFunction::BernoulliPdfFunction(std::shared_ptr<SimpleGrid> grid, std::weak_ptr<BetaPdfFunction> beta)
	: FunctionBase(grid)
	, m_mu(1.0)
{
	if (auto pBeta = beta.lock())
	{
		m_beta = std::make_unique<BetaPdfFunction>(*pBeta.get());
	}
}

void BernoulliPdfFunction::Fit(Eigen::MatrixXd data)
{
	int ones = data.sum();
	int zeros = data.size() - data.sum();

	if (m_beta)
	{
		m_beta->m_a += zeros; m_beta->m_b += ones;
	}
	else
	{
		m_mu = data.mean();
	}
}

int BernoulliPdfFunction::GetSample(const int sampleSize)
{
	std::random_device seed_gen;
	std::default_random_engine engine(seed_gen());
	std::uniform_real_distribution<> dist(0.0, 1.0);

	int count = 0;
	if (m_beta)
	{
		const double threshold = m_beta->m_b / (m_beta->m_a + m_beta->m_b);
		for (int i = 0; i < sampleSize; i++)
		{
			double result = dist(engine);
			count += threshold > result ? 1 : 0;
		}

		return count;
	}

	for (int i = 0; i < sampleSize; i++)
	{
		double result = dist(engine);
		count += m_mu > result ? 1 : 0;
	}

	return count;
}

double BernoulliPdfFunction::Calculate(const double x)
{
	if (m_beta)
	{
		return m_beta->Calculate(x);
	}

    return Math::Pow(m_mu, x) * Math::Pow(1 - m_mu, 1 - x);
}

