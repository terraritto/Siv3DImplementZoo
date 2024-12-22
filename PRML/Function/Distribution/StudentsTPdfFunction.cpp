#include "StudentsTPdfFunction.h"
#include "../../PRMLUtil.h"

StudentsTPdfFunction::StudentsTPdfFunction(std::shared_ptr<SimpleGrid> grid, Eigen::MatrixXd mu, Eigen::MatrixXd tau, Eigen::MatrixXd dof)
	: FunctionBase(grid)
	, m_mu(mu)
	, m_tau(tau)
	, m_dof(dof)
{
}

void StudentsTPdfFunction::Fit(Eigen::MatrixXd data, double learning_rate)
{
	// muの設定
	m_mu = Eigen::MatrixXd(data.cols(), 1);
	for (int i = 0; i < data.cols(); i++)
	{
		m_mu(i, 0) = data.col(i).mean();
	}

	// tauの設定
	Eigen::MatrixXd centered = data.rowwise() - data.colwise().mean();
	Eigen::MatrixXd cov = (centered.adjoint() * centered) / double(data.rows() - 1);
	m_tau = 1.0 / cov.array();

	// dofの設定
	m_dof = ConvertMatrixFromNumeric(1.0);

	Eigen::MatrixXd ex(m_mu.rows() + m_tau.rows() + m_dof.rows(), m_mu.cols());
	ex << m_mu, m_tau, m_dof;

	while (true)
	{
		auto [eta, lneta] = Expectation(data);
		Maximization(data, eta, lneta, learning_rate);

		Eigen::MatrixXd newParams(m_mu.rows() + m_tau.rows() + m_dof.rows(), m_mu.cols());
		newParams << m_mu, m_tau, m_dof;

		bool isCollected = (ex - newParams).isMuchSmallerThan(3, 0.1);
		if (isCollected)
		{
			break;
		}

		ex = newParams;
	}
}

Eigen::MatrixXd StudentsTPdfFunction::Pdf(Eigen::MatrixXd data) const
{
	Eigen::MatrixXd d = data.array() - m_mu(0, 0);

	Eigen::MatrixXd Factor1 = (d.array() * d.array() * m_tau(0, 0));

	Eigen::MatrixXd gammaData1 = 0.5 * (m_dof.array() + 1);
	for (int i = 0; i < gammaData1.rows(); i++)
	{
		for (int j = 0; j < gammaData1.cols(); j++)
		{
			gammaData1(i, j) = tgamma(gammaData1(i, j));
		}
	}

	Eigen::MatrixXd gammaData2 = 0.5 * m_dof.array();
	for (int i = 0; i < gammaData2.rows(); i++)
	{
		for (int j = 0; j < gammaData2.cols(); j++)
		{
			gammaData2(i, j) = tgamma(gammaData2(i, j));
		}
	}

	Eigen::MatrixXd Factor2 = (1 + Factor1.array() / m_dof(0, 0)).pow(-0.5 * (1.0 + m_dof(0, 0)));

	Eigen::MatrixXd Factor3 = (M_PI * m_dof.array()).pow(0.5);

	return gammaData1(0,0) * Pow(m_tau(0, 0), 0.5) * Factor2.array()
		/ gammaData2(0,0) / Factor3(0,0);
}

std::pair<Eigen::MatrixXd, Eigen::MatrixXd> StudentsTPdfFunction::Expectation(Eigen::MatrixXd data)
{
	Eigen::MatrixXd d = data.array() - m_mu(0,0);
	Eigen::MatrixXd a = 0.5 * (m_dof.array() + 1.0);
	Eigen::MatrixXd b = 0.5 * (m_dof(0,0) + m_tau(0,0) * d.array() * d.array());
	Eigen::MatrixXd eta = a(0,0) /  b.array();

	Eigen::MatrixXd digammaData = a;
	for (int i = 0; i < digammaData.rows(); i++)
	{
		for (int j = 0; j < digammaData.cols(); j++)
		{
			digammaData(i, j) = digamma(digammaData(i, j));
		}
	}

	Eigen::MatrixXd lneta = digammaData.array() - a.array().log();

	return std::make_pair(eta, lneta);
}

void StudentsTPdfFunction::Maximization(Eigen::MatrixXd data, Eigen::MatrixXd eta, Eigen::MatrixXd lneta, double learning_rate)
{
	// muの設定
	Eigen::MatrixXd etaSum(eta.cols(), 1);
	for (int i = 0; i < eta.cols(); i++)
	{
		etaSum(i, 0) = eta.col(i).mean();
	}

	Eigen::MatrixXd etaXData = eta.array() * data.array();
	Eigen::MatrixXd etaXSum(etaXData.cols(), 1);
	for (int i = 0; i < etaXData.cols(); i++)
	{
		etaXSum(i, 0) = etaXData.col(i).mean();
	}

	m_mu = etaXSum.array() / etaSum.array();

	Eigen::MatrixXd d = data.array() - m_mu(0, 0);

	// tauの設定
	Eigen::MatrixXd tauData = eta.array() * d.array() * d.array();
	Eigen::MatrixXd tauSum(tauData.cols(), 1);
	for (int i = 0; i < tauData.cols(); i++)
	{
		tauSum(i, 0) = tauData.col(i).mean();
	}
	m_tau = 1 / tauSum.array();

	// dofの設定
	double N = data.rows();

	Eigen::MatrixXd digammaData = 0.5 * m_dof;
	for (int i = 0; i < digammaData.rows(); i++)
	{
		for (int j = 0; j < digammaData.cols(); j++)
		{
			digammaData(i, j) = digamma(digammaData(i, j));
		}
	}

	Eigen::MatrixXd dofData = lneta(0,0) - eta.array();
	Eigen::MatrixXd dofSum(dofData.cols(), 1);
	for (int i = 0; i < dofData.cols(); i++)
	{
		dofSum(i, 0) = dofData.col(i).mean();
	}

	Eigen::MatrixXd dofLog = (m_dof * 0.5).array().log();

	m_dof = m_dof.array() + learning_rate * 0.5 *
		(N * dofLog.array() + N
		- N * digammaData.array() + dofSum.array());
}
