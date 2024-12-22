#pragma once
#include <Eigen/Dense>

class KernelBase
{
public:
	virtual Eigen::MatrixXd Call(Eigen::MatrixXd x, Eigen::MatrixXd y, bool isPairwise = true) = 0;

protected:
	// x,yを複製していく
	std::pair<Eigen::MatrixXd, Eigen::MatrixXd> PairWise(Eigen::MatrixXd x, Eigen::MatrixXd y);
};
