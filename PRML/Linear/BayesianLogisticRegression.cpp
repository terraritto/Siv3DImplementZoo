#include "BayesianLogisticRegression.h"

BayesianLogisticRegression::BayesianLogisticRegression(double alpha)
	: m_alpha(alpha)
{
}

void BayesianLogisticRegression::fit(Array<Array<double>> xTrain, Array<double> yTrain, int maxIter)
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

	Eigen::MatrixXd w(D, 1); w.setZero();
	Eigen::MatrixXd eye = Eigen::MatrixXd(D, D).setZero().setIdentity();

	m_mean = w; m_precision = m_alpha * eye;

	auto isDiff = [](double a, double b)
	{
		return Math::Abs(a - b) < 0.0001;
	};

	Eigen::MatrixXd hessian;
	for (int i = 0; i < maxIter; i++)
	{
		Eigen::MatrixXd w_prev = w;
		Eigen::MatrixXd y = Sigmoid(xt * w);
		Eigen::MatrixXd grad = xt.transpose() * (y - yt) + m_precision * (w - m_mean);
		Eigen::MatrixXd hessianFactor(xt.rows(), xt.cols());
		Eigen::MatrixXd factor = y.array() * (1 - y.array());
		for (int i = 0; i < xt.cols(); i++)
		{
			hessianFactor.col(i) = xt.col(i).array() * factor.array();
		}
		hessian = hessianFactor.transpose() * xt + m_precision;

		w -= hessian.colPivHouseholderQr().solve(grad);

		// 既に閉じてたら終わりにするように
		bool isClose = true;
		for (int i = 0; i < w.rows(); i++)
		{
			if (!isDiff(w(i, 0), w_prev(i, 0)))
			{
				isClose = false;
				break;
			}
		}

		if (isClose)
		{
			break;
		}
	}

	m_mean = w;
	m_precision = hessian;
}

Eigen::MatrixXd BayesianLogisticRegression::Proba(Array<Array<double>> xTest)
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

	Eigen::MatrixXd muA = xt * m_mean;
	Eigen::MatrixXd var = m_precision.colPivHouseholderQr().solve(xt.transpose()).transpose();
	var = var.array() * xt.array();
	Eigen::MatrixXd varA(var.rows(), 1);
	for (int i = 0; i < var.rows(); i++)
	{
		varA(i, 0) = var.row(i).sum();
	}

	Eigen::MatrixXd factor = muA.array() / (Math::Pi * varA.array()).sqrt();
	Eigen::MatrixXd sigmoidData = Sigmoid(factor);

	// 数値が一番でかいものが対象のクラス
	Eigen::MatrixXd result(N, 1);
	for (int i = 0; i < sigmoidData.rows(); i++)
	{
		result(i, 0) = sigmoidData(i, 0);
	}

	return result;
}
