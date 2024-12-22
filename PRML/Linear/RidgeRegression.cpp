#include "RidgeRegression.h"

RidgeRegression::RidgeRegression(double alpha)
	: m_alpha(alpha)
{
}

void RidgeRegression::fit(Array<Array<double>> xTrain, Array<double> yTrain)
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

	// 単位ベクトルの生成
	Eigen::MatrixXd I(D, D);
	I.setOnes();

	// AX=BのA,Bを生成
	Eigen::MatrixXd A(D, D); Eigen::MatrixXd B(D, 1);
	A = m_alpha * I + xt.transpose() * xt;
	B = xt.transpose() * yt;

	// Xを算出
	m_weight = A.colPivHouseholderQr().solve(B);
}

Array<double> RidgeRegression::Predict(Array<Array<double>> xTest)
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
	Eigen::MatrixXd result = xt * m_weight;

	// 元に戻す
	Array<double> resultData;
	for (size_t n = 0; n < N; n++)
	{
		resultData.push_back(result(n, 0));
	}

	return resultData;
}
