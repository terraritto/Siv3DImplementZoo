#pragma once
#include <Siv3D.hpp>
#include <Eigen/Dense>

class LabelTransformer
{
public:
	LabelTransformer(int n = -1);

	Eigen::MatrixXd Encode(Array<double> data);

	[[nodiscard]]
	int GetClassNum() const { return m_classNum; }

	[[nodiscard]]
	Eigen::MatrixXd GetEncoder() const { return m_encoder; }
		
	void SetClassNum(int n);

protected:
	int m_classNum;
	Eigen::MatrixXd m_encoder;
};
