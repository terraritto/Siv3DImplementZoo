#include "KernelBase.h"

std::pair<Eigen::MatrixXd, Eigen::MatrixXd> KernelBase::PairWise(Eigen::MatrixXd x, Eigen::MatrixXd y)
{
	// Input: x->(N,1) y->(M,1)
	// Output x->(N,M) y->(N,M) 単純にコピー、とりあえずこれで様子見
	Eigen::MatrixXd xt(x.rows(), y.rows());
	Eigen::MatrixXd yt(x.rows(), y.rows());

	for (int i = 0; i < x.rows(); i++)
	{
		for (int j = 0; j < y.rows(); i++)
		{
			xt(i, j) = x(i, 0);
			yt(i, j) = y(i, 0);
		}
	}

	return { xt, yt };
}
