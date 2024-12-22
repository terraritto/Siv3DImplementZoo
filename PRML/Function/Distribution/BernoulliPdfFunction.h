#pragma once
#include "../../FunctionBase.h"
#include "BetaPdfFunction.h"
#include <Eigen/Dense>

class BernoulliPdfFunction : public FunctionBase
{
public:
	BernoulliPdfFunction(std::shared_ptr<SimpleGrid> grid, double mu = 1.0);
	BernoulliPdfFunction(std::shared_ptr<SimpleGrid> grid, std::weak_ptr<BetaPdfFunction> beta);

	void Fit(Eigen::MatrixXd data);

	[[nodiscard]]
	int GetSample(const int sampleSize);

protected:
	virtual double Calculate(const double x) final;

	std::unique_ptr<BetaPdfFunction> m_beta;

	double m_mu;
};
