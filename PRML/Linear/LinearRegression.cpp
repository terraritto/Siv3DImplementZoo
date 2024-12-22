#include "LinearRegression.h"

void LinearRegression::fit(Array<Array<double>> xTrain, Array<double> yTrain)
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

	// 疑似逆行列を計算
	// まずは特異値分解
	Eigen::JacobiSVD<Eigen::MatrixXd> svd(xt, Eigen::ComputeThinU | Eigen::ComputeThinV);

	// S.Tを計算
	Eigen::VectorXd s = svd.singularValues();
	s = s.array().inverse();

	// A+ = U * S.T * U.Tで行ける(A+が疑似逆行列
	Eigen::MatrixXd xtPseudoInv = svd.matrixV() * s.asDiagonal() * svd.matrixU().transpose();//.transpose();

	// 必要な要素を計算
	m_weight = xtPseudoInv * yt;
	m_var = (xt * m_weight - yt).cwiseSqrt().mean();
}

Array<double> LinearRegression::Predict(Array<Array<double>> xTest)
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
