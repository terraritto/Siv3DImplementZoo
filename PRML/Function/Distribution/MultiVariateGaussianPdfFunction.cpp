#include "MultiVariateGaussianPdfFunction.h"

MultiVariateGaussianPdfFunction::MultiVariateGaussianPdfFunction(std::shared_ptr<SimpleGrid> grid, Eigen::MatrixXd mu, Eigen::MatrixXd cov)
	: FunctionBase(grid)
	, m_mu(mu)
	, m_cov(cov)
{
}

void MultiVariateGaussianPdfFunction::Fit(Eigen::MatrixXd data)
{
	m_mu = Eigen::MatrixXd(data.cols(), 1);

	for (int i = 0; i < data.cols(); i++)
	{
		m_mu(i, 0) = data.col(i).mean();
	}

	// 共分散行列の計算
	Eigen::MatrixXd centered = data.rowwise() - data.colwise().mean();
	Eigen::MatrixXd cov = (centered.adjoint() * centered) / double(data.rows() - 1);
	m_cov = cov;
	m_tau = cov.inverse();
}

Eigen::MatrixXd MultiVariateGaussianPdfFunction::Pdf(Eigen::MatrixXd data) const
{
	Eigen::MatrixXd d(data.rows(), data.cols());// = data.array() - m_mu.array();
	for (int i = 0; i < data.rows(); i++)
	{
		d(i, 0) = data(i, 0) - m_mu(0, 0);
		d(i, 1) = data(i, 1) - m_mu(1, 0);
	}

	Eigen::MatrixXd Factor1 = (d * m_tau).array() * d.array();
	Eigen::MatrixXd Factor2(data.rows(), 1);
	for (int i = 0; i < Factor1.rows(); i++)
	{
		Factor2(i, 0) = Factor1.row(i).sum();
	}

	Factor2 *= -0.5f;
	Factor2 = Eigen::exp(Factor2.array());

	double Factor3 = Math::Sqrt(m_tau.determinant());
	double Factor4 = Math::Pow(2 * M_PI, 0.5 * m_mu.size());

	return Factor2 * Factor3 / Factor4;
}
