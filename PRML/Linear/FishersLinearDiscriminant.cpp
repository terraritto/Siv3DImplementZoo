#include "FishersLinearDiscriminant.h"
#include <unsupported/Eigen/Polynomials>
#include "./../Function/Distribution/GaussianPdfFunction.h"

FisaherLinearDiscriminant::FisaherLinearDiscriminant()
{
}

void FisaherLinearDiscriminant::fit(Array<Array<double>> xTrain, Array<double> yTrain)
{
	const size_t N = xTrain.size();
	const size_t D = xTrain[0].size();

	Array<int> A, B;
	for (int i = 0; i < yTrain.size(); i++)
	{
		if (yTrain[i] < 0.5)
		{
			A.push_back(i);
		}
		else
		{
			B.push_back(i);
		}
	}

	// 0,1でデータを振り分け
	Eigen::MatrixXd x0(A.size(), D);
	Eigen::MatrixXd x1(B.size(), D);
	for (int i = 0; i < A.size(); i++)
	{
		int n = A[i];
		for (size_t d = 0; d < D; d++)
		{
			x0(i, d) = xTrain[n][d];
		}
	}
	for (int i = 0; i < B.size(); i++)
	{
		int n = B[i];
		for (size_t d = 0; d < D; d++)
		{
			x1(i, d) = xTrain[n][d];
		}
	}

	// 平均用行列
	Eigen::MatrixXd m0(x0.cols(), 1);
	Eigen::MatrixXd m1(x1.cols(), 1);
	for (int i = 0; i < x0.cols(); i++)
	{
		m0(i, 0) = x0.col(i).mean();
	}

	for (int i = 0; i < x1.cols(); i++)
	{
		m1(i, 0) = x1.col(i).mean();
	}

	// 共分散行列の計算
	Eigen::MatrixXd centered0 = x0.rowwise() - x0.colwise().mean();
	Eigen::MatrixXd centered1 = x1.rowwise() - x1.colwise().mean();
	Eigen::MatrixXd cov0 = (x0.adjoint() * x0) / double(x0.rows() - 1);
	Eigen::MatrixXd cov1 = (x1.adjoint() * x1) / double(x1.rows() - 1);
	Eigen::MatrixXd cov = cov0 + cov1;

	// 重みを算出
	m_w = cov.colPivHouseholderQr().solve(m1 - m0);

	auto g0 = std::make_shared<GaussianPdfFunction>();
	g0->Fit(x0 * m_w);
	auto g1 = std::make_shared<GaussianPdfFunction>();
	g1->Fit(x1 * m_w);

	Eigen::MatrixXd root00 = (g1->GetCov().array() / g0->GetCov().array()).log();
	Eigen::MatrixXd root01 = g0->GetCov() - g1->GetCov();
	Eigen::MatrixXd root02 = 2 * (g0->GetCov() * g1->GetMuBase() - g1->GetCov() * g0->GetMuBase());
	Eigen::MatrixXd root03 = g1->GetCov() * g0->GetMuBase() * g0->GetMuBase() - g0->GetCov() * g1->GetMuBase() * g1->GetMuBase()
		- g1->GetCov() * g0->GetCov() * root00;
	Eigen::Vector3d root; root << root01(0,0), root02(0,0), root03(0,0);

	// rootの計算
	Eigen::PolynomialSolver<double, Eigen::Dynamic> solver;
	Eigen::VectorXd data;
	solver.compute(root);
	const Eigen::PolynomialSolver<double, Eigen::Dynamic>::RootsType& r = solver.roots();
	double result = root(0);

	double g0Data = g0->GetMuBase()(0, 0);
	double g1Data = g1->GetMuBase()(0, 0);
	if (
		(g0Data < result && result < g1Data) ||
		(g1Data < result && result < g0Data)
		)
	{
		m_threshold = root(0);
	}
	else
	{
		m_threshold = root(1);
	}
}

Eigen::MatrixXd FisaherLinearDiscriminant::classify(Array<Array<double>> xTest)
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
		result(i, 0) = xt(i, 0) > m_threshold ? 1.0 : 0.0;
	}

	return result;
}
