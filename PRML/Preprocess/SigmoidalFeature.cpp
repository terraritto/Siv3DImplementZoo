#include "SigmoidalFeature.h"

SigmoidalFeature::SigmoidalFeature(Eigen::MatrixXd mean, double coef)
	: m_mean(mean)
	, m_coef(coef)
{
}
