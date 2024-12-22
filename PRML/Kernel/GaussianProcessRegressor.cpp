#include "GaussianProcessRegressor.h"

GaussianProcessRegressor::GaussianProcessRegressor(std::shared_ptr<KernelBase> kernel, double beta)
	: m_kernel(kernel)
	, m_beta(beta)
{
}

void GaussianProcessRegressor::fit(Array<double> xTrain, Array<double> yTrain, int maxIter, double learningRate)
{
	/*
	const size_t N = xTrain.size();
	Eigen::MatrixXd xt(N, 1);
	Eigen::MatrixXd yt(N, 1);

	// データを設定
	for (size_t n = 0; n < N; n++)
	{
		xt(n, 0) = xTrain[n];
		yt(n, 0) = yTrain[n];
	}

	// x,tを保持
	m_x = xt; m_t = yt;

	Eigen::MatrixXd eye = Eigen::MatrixXd(N, N).setZero().setIdentity();
	Eigen::MatrixXd Gram = m_kernel->Call(xt, yt); // @todo: kernelを取るように

	m_cov = Gram + eye / m_beta;
	m_precision = m_cov.inverse();

	// @todo: logkikelihoodの拡張
	for (int i = 0; i < maxIter; i++)
	{
	}
	*/
}
