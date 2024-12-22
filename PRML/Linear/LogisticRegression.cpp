#include "LogisticRegression.h"

void LogisticRegression::fit(Array<Array<double>> xTrain, Array<double> yTrain, int maxIter)
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

	Eigen::MatrixXd w(D, 1);
	w.setZero();

	auto isDiff = [](double a, double b)
	{
		return Math::Abs(a - b) < 0.0001;
	};

	for (int i = 0; i < maxIter; i++)
	{
		Eigen::MatrixXd prev = w;
		Eigen::MatrixXd y = Sigmoid(xt * w);
		Eigen::MatrixXd grad = xt.transpose() * (y - yt);
		Eigen::MatrixXd hessianFactor(xt.rows(), xt.cols());
		Eigen::MatrixXd factor = y.array() * (1 - y.array());
		for (int i = 0; i < xt.cols(); i++)
		{
			hessianFactor.col(i) = xt.col(i).array() * factor.array();
		}
		Eigen::MatrixXd hessian = hessianFactor.transpose() * xt;

		// Xを算出
		w -= hessian.colPivHouseholderQr().solve(grad);

		// 既に閉じてたら終わりにするように
		bool isClose = true;
		for (int i = 0; i < w.rows(); i++)
		{
			if (!isDiff(w(i, 0), prev(i, 0)))
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

	m_w = w;
}

Eigen::MatrixXd LogisticRegression::classify(Array<Array<double>> xTest, double threshold)
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
	xt = Sigmoid(xt * m_w);

	// 数値が一番でかいものが対象のクラス
	Eigen::MatrixXd result(N, 1);
	for (int i = 0; i < xt.rows(); i++)
	{
		Eigen::VectorXf::Index maxId;
		xt.row(i).maxCoeff(&maxId);
		result(i, 0) = xt(i, 0) > threshold ? 1.0 : 0.0;
	}

	return result;
}

Eigen::MatrixXd LogisticRegression::Proba(Array<Array<double>> xTest)
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
	xt = Sigmoid(xt * m_w);

	// 数値が一番でかいものが対象のクラス
	Eigen::MatrixXd result(N, 1);
	for (int i = 0; i < xt.rows(); i++)
	{
		result(i, 0) = xt(i, 0);
	}

	return result;
}

Eigen::MatrixXd LogisticRegression::Sigmoid(Eigen::MatrixXd a)
{
	Eigen::MatrixXd base = a * 0.5;
	Eigen::MatrixXd expPlus = base.array().exp();
	Eigen::MatrixXd expMinus = (base * -1.0).array().exp();
	Eigen::MatrixXd tanh = (expPlus.array() - expMinus.array()) / (expPlus.array() + expMinus.array());

	return (tanh.array() * 0.5 + 0.5);
}
