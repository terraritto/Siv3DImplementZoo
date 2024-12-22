#include "Polynomial.h"

/*
Eigen::MatrixXd PolynomialKernel::Call(Eigen::MatrixXd x, Eigen::MatrixXd y, bool isPairwise)
{
	const int N = x.rows();

	if (isPairwise)
	{
		auto [xt, yt] = PairWise(x, y);
		x = xt; y = yt;
	}

	auto mulxy = x.array() * y.array();

	// とりあえずPythonと計算値が合うように全部同じ値になるようにする
	auto sumValue = mulxy.rows();
}
*/
