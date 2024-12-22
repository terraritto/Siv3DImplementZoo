#include "LabelTransformer.h"

LabelTransformer::LabelTransformer(int n)
{
	SetClassNum(n);
}

Eigen::MatrixXd LabelTransformer::Encode(Array<double> data)
{
	if (m_classNum == -1)
	{
		double maxValue = *(std::max_element(data.begin(), data.end()));
		SetClassNum(maxValue + 1);
	}

	//[N,]->[N,K]のサイズに
	// K=2の場合,[[1,0],[0,1]]なので、
	// data=[0,0,1,1]とすると、[[1,0],[1,0],[0,1],[0,1]]となる
	Eigen::MatrixXd result(data.size(), m_classNum);
	for (int i = 0; i < data.size(); i++)
	{
		for (int j = 0; j < m_classNum; j++)
		{
			result(i, j) = m_encoder(data[i], j);
		}
	}

	return result;
}

void LabelTransformer::SetClassNum(int n)
{
	m_classNum = n;

	if (n == -1)
	{
		m_encoder = Eigen::MatrixXd();
		return;
	}

	Eigen::MatrixXd eye(n, n);
	eye.setZero().setIdentity();
	m_encoder = eye;
}
