#include "GaussianFeature.h"

GaussianFeature::GaussianFeature(Eigen::MatrixXd mean, double var)
	: m_mean(mean)
	, m_var(var)
{
}
