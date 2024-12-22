#include "GaussianPdfFunction.h"
#include "../../PRMLUtil.h"

GaussianPdfFunction::GaussianPdfFunction(std::shared_ptr<SimpleGrid> grid, Eigen::MatrixXd mu, Eigen::MatrixXd cov)
	: FunctionBase(grid)
	, m_mu(mu)
	, m_cov(cov)
{
	if (IsScalar(m_cov))
	{
		m_tau = 1 / m_cov.array();
	}
}

GaussianPdfFunction::GaussianPdfFunction(std::shared_ptr<SimpleGrid> grid, std::weak_ptr<GaussianPdfFunction> mu, Eigen::MatrixXd cov)
	: FunctionBase(grid)
	, m_muGauss(mu)
	, m_cov(cov)
{
	if (IsScalar(m_cov))
	{
		m_tau = 1 / m_cov.array();
	}
}

GaussianPdfFunction::GaussianPdfFunction(std::shared_ptr<SimpleGrid> grid, Eigen::MatrixXd mu, std::weak_ptr<GammaPdfFunction> tau)
	: FunctionBase(grid)
	, m_mu(mu)
	, m_tauGamma(tau)
{
}

void GaussianPdfFunction::Fit(Eigen::MatrixXd data)
{
	if (m_muGauss.expired() == false)
	{
		BayesMu(data);
		return;
	}

	if (m_tauGamma.expired() == false)
	{
		BayesTau(data);
		return;
	}

	Mu(data);
}

void GaussianPdfFunction::BayesMu(Eigen::MatrixXd data)
{
	int N = data.rows();

	auto pGauss = m_muGauss.lock();

	Eigen::MatrixXd mu = Eigen::MatrixXd(data.cols(), 1);
	for (int i = 0; i < data.cols(); i++)
	{
		mu(i, 0) = data.col(i).mean();
	}

	double tau = pGauss->m_tau(0,0) + N * m_tau(0,0);

	Eigen::MatrixXd nextMu, nextTau;
	nextMu = (pGauss->m_mu * pGauss->m_tau(0,0) + N * mu * m_tau(0, 0)) / tau;
	nextTau = ConvertMatrixFromNumeric(tau);

	pGauss->SetMu(nextMu); pGauss->SetTau(nextTau);
}

void GaussianPdfFunction::BayesTau(Eigen::MatrixXd data)
{
	int N = data.rows();

	auto pGamma = m_tauGamma.lock();

	// 共分散行列の計算
	Eigen::MatrixXd centered = data.rowwise() - data.colwise().mean();
	Eigen::MatrixXd cov = (centered.adjoint() * centered) / double(data.rows() - 1);

	double nextA, nextB;
	nextA = pGamma->GetA() + 0.5 * N;
	nextB = pGamma->GetB() + 0.5 * N * cov(0, 0);

	pGamma->SetA(nextA); pGamma->SetB(nextB);
}

void GaussianPdfFunction::Mu(Eigen::MatrixXd data)
{
	m_mu = Eigen::MatrixXd(data.cols(), 1);
	for (int i = 0; i < data.cols(); i++)
	{
		m_mu(i, 0) = data.col(i).mean();
	}

	// 共分散行列の計算
	Eigen::MatrixXd centered = data.rowwise() - data.colwise().mean();
	Eigen::MatrixXd cov = (centered.adjoint() * centered) / double(data.rows() - 1);
	SetCov(cov);
}

Eigen::MatrixXd GaussianPdfFunction::Pdf(Eigen::MatrixXd data) const
{
	auto pGauss = m_muGauss.lock();
	Eigen::MatrixXd d = data.array() - (pGauss ? pGauss->m_mu(0, 0) : m_mu(0, 0));

	Eigen::MatrixXd Factor1 = (d.array() * d.array() * m_tau(0, 0));

	Factor1 *= -0.5;
	Factor1 = Eigen::exp(Factor1.array());

	double Factor2 = Math::Sqrt(2 * M_PI * m_cov(0, 0));

	return Factor1 / Factor2;
}
