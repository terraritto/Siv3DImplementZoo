#include "SoftmaxRegression.h"
#include "../Preprocess/LabelTransformer.h"

SoftmaxRegression::SoftmaxRegression()
{
}

void SoftmaxRegression::fit(Array<Array<double>> xTrain, Array<double> yTrain, int maxIter, double learingRate)
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

	auto isDiff = [](double a, double b)
	{
		return Math::Abs(a - b) < 0.0001;
	};

	Eigen::MatrixXd label = LabelTransformer().Encode(yTrain);

	Eigen::MatrixXd w(D, label.cols()); w.setZero();

	for (int i = 0; i < maxIter; i++)
	{
		Eigen::MatrixXd wPrev = w;
		Eigen::MatrixXd yPrev = xt * w;
		Eigen::MatrixXd y = Softmax(yPrev);

		Eigen::MatrixXd grad = xt.transpose() * (y - label);
		w = w - learingRate * grad;

		// 既に閉じてたら終わりにするように
		bool isClose = true;
		for (int j = 0; j < w.rows(); j++)
		{
			for (int k = 0; k < w.cols(); k++)
			{
				if (!isDiff(w(j, k), wPrev(j, k)))
				{
					isClose = false;
					break;
				}
			}
		}

		if (isClose)
		{
			break;
		}
	}

	m_w = w;
}

Eigen::MatrixXd SoftmaxRegression::classify(Array<Array<double>> xTest)
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
	xt = Softmax(xt * m_w);

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

Eigen::MatrixXd SoftmaxRegression::Softmax(Eigen::MatrixXd a)
{
	/*
	Eigen::MatrixXd aMax(a.rows(), 1), sumA(a.rows(), 1);

	for (int i = 0; i < a.rows(); i++)
	{
		aMax(i, 0) = a.row(i).maxCoeff();
	}

	Eigen::MatrixXd expA(a.rows(), a.cols());
	for (int i = 0; i < a.cols(); i++)
	{
		expA.col(i) = a.col(i) - aMax.col(0);
	}
	expA = expA.array().exp();

	for (int i = 0; i < a.rows(); i++)
	{
		sumA(i, 0) = expA.row(i).sum();
	}

	Eigen::MatrixXd result(a.rows(), a.cols());
	for (int i = 0; i < a.cols(); i++)
	{
		result.col(i) = expA.col(i).array() / sumA.col(0).array();
	}

	return result;
	*/

	for (int i = 0; i < a.rows(); i++)
	{
		a.row(i) = a.row(i).unaryExpr([&](double p) {return exp(p - a.row(i).maxCoeff()); });
		a.row(i).array() /= a.row(i).sum();
	}

	return a;
}
