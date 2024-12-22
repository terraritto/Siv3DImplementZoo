#pragma once
#include <Siv3D.hpp>
#include <Eigen/Dense>
#include "../PRMLUtil.h"

class GaussianFeature
{
public:
	GaussianFeature(Eigen::MatrixXd mean, double var);

	template<class T>
	Array<Array<T>> Transform(const Eigen::MatrixXd& data)
	{
		Array<Array<T>> result;
		result.resize(data.size());

		// 最初の項は1で埋める
		for (int i = 0; i < data.size(); i++)
		{
			result[i].push_back(1);
		}

		for (int i = 0; i < data.size(); i++)
		{
			for (int j = 0; j < m_mean.rows(); j++)
			{
				Eigen::MatrixXd temp = data;
				double factor = Math::Pow(temp(i, 0) - m_mean(j, 0), 2.0);
				double gauss = Math::Exp(-0.5 * (factor) / m_var);
				result[i].push_back(gauss);
			}
		}

		return result;
	}

private:
	Eigen::MatrixXd m_mean;
	double m_var;
};
