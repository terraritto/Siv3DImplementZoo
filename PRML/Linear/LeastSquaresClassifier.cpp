#include "LeastSquaresClassifier.h"
#include "../Preprocess/LabelTransformer.h"

LeastSquaresClassifier::LeastSquaresClassifier(Eigen::MatrixXd w)
	: m_w(w)
{
}

void LeastSquaresClassifier::fit(Array<Array<double>> xTrain, Array<double> yTrain)
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

	Eigen::MatrixXd label = LabelTransformer().Encode(yTrain);

	// 疑似逆行列を計算
	// まずは特異値分解
	Eigen::JacobiSVD<Eigen::MatrixXd> svd(xt, Eigen::ComputeThinU | Eigen::ComputeThinV);

	// S.Tを計算
	Eigen::VectorXd s = svd.singularValues();
	s = s.array().inverse();

	// A+ = U * S.T * U.Tで行ける(A+が疑似逆行列
	Eigen::MatrixXd xtPseudoInv = svd.matrixV() * s.asDiagonal() * svd.matrixU().transpose();//.transpose();


	m_w = xtPseudoInv * label;
}

Eigen::MatrixXd LeastSquaresClassifier::classify(Array<Array<double>> xTest, int sample)
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
	xt = xt * m_w;

	// 数値が一番でかいものが対象のクラス
	Eigen::MatrixXd result(N, 1);
	for (int i = 0; i < xt.rows(); i++)
	{
		Eigen::VectorXf::Index maxId;
		xt.row(i).maxCoeff(&maxId);
		result(i, 0) = maxId;
	}

	return result;
}
