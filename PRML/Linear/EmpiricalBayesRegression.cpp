#include "EmpiricalBayesRegression.h"

void EmpiricalBayesRegression::fit(Array<Array<double>> xTrain, Array<double> yTrain, int maxIter)
{
	const size_t N = xTrain.size();
	const size_t D = xTrain[0].size();
	Eigen::MatrixXd xt(N, D);
	Eigen::MatrixXd yt(N, 1);

	// サイズを確定
	if (!IsPriorDefined())
	{
		m_mean = std::allocate_shared<Eigen::MatrixXd>(Eigen::aligned_allocator<Eigen::MatrixXd>());
		m_precision = std::allocate_shared<Eigen::MatrixXd>(Eigen::aligned_allocator<Eigen::MatrixXd>());
	}

	// データを設定
	for (size_t n = 0; n < N; n++)
	{
		for (size_t d = 0; d < D; d++)
		{
			xt(n, d) = xTrain[n][d];
		}
		yt(n, 0) = yTrain[n];
	}

	Eigen::MatrixXd xtx = xt.transpose() * xt;
	auto eigenValues = xtx.eigenvalues();
	Eigen::MatrixXd eye = Eigen::MatrixXd(D, D).setZero().setIdentity();

	auto isDiff = [](double a, double b)
	{
		return Math::Abs(a - b) < 0.0001;
	};

	Eigen::MatrixXd w_mean, w_precision;

	for (int i = 0; i < maxIter; i++)
	{
		double alpha = m_alpha;
		double beta = m_beta;

		// AX=BのAを生成
		w_precision = m_alpha * eye + m_beta * xtx;

		// AX=BのBを生成
		Eigen::MatrixXd B(D, 1);
		B = xt.transpose() * yt;

		// Xを算出
		w_mean = m_beta * w_precision.colPivHouseholderQr().solve(B);
		double gamma = (eigenValues.array() / (m_alpha + eigenValues.array())).sum().real();
		m_alpha = gamma / Max(w_mean.array().pow(2.0).sum(), 1e-10);
		m_beta = (N - gamma) / (yt - xt * w_mean).array().pow(2.0).sum();

		if (isDiff(m_alpha, alpha) && isDiff(m_beta, beta))
		{
			break;
		}
	}

	*m_mean = w_mean;
	*m_precision = w_precision;
	m_conv = w_precision.inverse();
}

double EmpiricalBayesRegression::LogEvidence(Array<Array<double>> xTrain, Array<double> yTrain) const
{
	const size_t N = xTrain.size();
	const size_t D = xTrain[0].size();
	Eigen::MatrixXd xt(N, D);
	Eigen::MatrixXd yt(N, 1);

	// データを設定
	for (size_t n = 0; n < N; n++)
	{
		for (size_t d = 0; d < D; d++)
		{
			xt(n, d) = xTrain[n][d];
		}
		yt(n, 0) = yTrain[n];
	}

	double det = (*m_precision).determinant();
	auto sign = Math::Sign(det);
	det = Math::Log(Math::Abs(det));

	auto Factor1 = D * Math::Log(m_alpha) + N * Math::Log(m_beta);
	auto Factor2 = static_cast<double>(sign) * det + D * Math::Log(Math::TwoPi);
	auto Factor3 = LogPosterior(xt, yt, *m_mean);

	return 0.5 * (Factor1 - Factor2) + Factor3;
}

double EmpiricalBayesRegression::LogPrior(Eigen::MatrixXd w) const
{
	return -0.5 * m_alpha * Math::Square(w.sum());
}

double EmpiricalBayesRegression::LogLikelihood(Eigen::MatrixXd x, Eigen::MatrixXd y, Eigen::MatrixXd w) const
{
	return -0.5 * m_beta * (y - x * w).array().pow(2.0).sum();
}

double EmpiricalBayesRegression::LogPosterior(Eigen::MatrixXd x, Eigen::MatrixXd y, Eigen::MatrixXd w) const
{
	return LogLikelihood(x,y,w) + LogPrior(w);
}
