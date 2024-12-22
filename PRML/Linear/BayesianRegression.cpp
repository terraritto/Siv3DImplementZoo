#include "BayesianRegression.h"
#include "../eigenmvn.h"

BayesianRegression::BayesianRegression(double alpha, double beta)
	: m_alpha(alpha)
	, m_beta(beta)
{
}

void BayesianRegression::fit(Array<Array<double>> xTrain, Array<double> yTrain)
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

	if (!IsPriorDefined())
	{
		m_mean = std::allocate_shared<Eigen::MatrixXd>(Eigen::aligned_allocator<Eigen::MatrixXd>());
		m_precision = std::allocate_shared<Eigen::MatrixXd>(Eigen::aligned_allocator<Eigen::MatrixXd>());

		// サイズを確定
		*m_mean = Eigen::MatrixXd(D, 1);
		*m_precision = Eigen::MatrixXd(D, D);

		m_mean->setZero();
		*m_precision = m_precision->setIdentity().array() * m_alpha;
	}

	Eigen::MatrixXd prevPrecision = *m_precision;
	Eigen::MatrixXd prevMean = *m_mean;

	*m_precision = prevPrecision + m_beta * xt.transpose() * xt;

	// AX=BのA,Bを生成
	Eigen::MatrixXd A(D, D); Eigen::MatrixXd B(D, 1);
	A = *m_precision;
	B = prevPrecision * prevMean + m_beta * xt.transpose() * yt;

	// Xを算出
	*m_mean = A.colPivHouseholderQr().solve(B);
	m_conv = m_precision->inverse();
}

Array<double> BayesianRegression::Predict(Array<Array<double>> xTest, int sample)
{
	const size_t N = xTest.size();
	const size_t D = xTest[0].size();
	Eigen::MatrixXd xt(N, D);

	// データを設定
	for (size_t n = 0; n < N; n++)
	{
		for (size_t d = 0; d < D; d++)
		{
			xt(n, d) = xTest[n][d];
		}
	}

	// 予測
	Eigen::MatrixXd result;
	if (sample != 1)
	{
		result = Eigen::MatrixXd(xt.rows(), sample);
		Eigen::EigenMultivariateNormal<double> normX_solver(*m_mean, m_conv);
		Eigen::MatrixXd sampleData = normX_solver.samples(sample);
		result = xt * sampleData;
	}
	else
	{
		result = xt * (*m_mean);
	}


	// 元に戻す
	Array<double> resultData;
	for (int j = 0; j < sample; j++)
	{
		for (size_t n = 0; n < N; n++)
		{
			resultData.push_back(result(n, j));
		}
	}

	return resultData;
}

bool BayesianRegression::IsPriorDefined() const
{
	return m_precision && m_mean;
}
