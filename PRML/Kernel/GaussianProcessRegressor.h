#pragma once
#include <Siv3D.hpp>
#include <Eigen/Dense>
#include "KernelBase.h"

class GaussianProcessRegressor
{
public:
	GaussianProcessRegressor(std::shared_ptr<KernelBase> kernel, double beta);

	void fit(Array<double> xTrain, Array<double> yTrain, int maxIter = 0, double learningRate = 0.1);

protected:
	Eigen::MatrixXd m_x, m_t;
	Eigen::MatrixXd m_cov, m_precision;

	std::shared_ptr<KernelBase> m_kernel;
	double m_beta;
};
