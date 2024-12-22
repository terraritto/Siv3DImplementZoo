#pragma once
#include <Siv3D.hpp>
#include <Eigen/Dense>
#include "../PRMLUtil.h"

class PolynomialFeature
{
public:
	PolynomialFeature(int degree = 2);

	// とりあえず一次元変換で考えてみる
	// degree 3の場合
	// [1,2,3] -> [[1,1,1,1],[1,2,4,8],[1,3,9,27]]
	template<class T>
	Array<Array<T>> Transform(const Array<T>& data)
	{
		Array<Array<T>> result;
		result.resize(data.size());

		// 最初の項は1で埋める
		for (int i = 0; i < data.size(); i++)
		{
			result[i].push_back(1);
		}

		// degree=1以降のものを計算
		for (int i = 0; i < data.size(); i++)
		{
			for (int d = 1; d < m_degree + 1; d++)
			{
				result[i].push_back(Math::Pow(data[i], static_cast<T>(d)));
			}
		}

		return result;
	}

	Array<Array<double>> Transform(const Eigen::MatrixXd data)
	{
		Array<Array<double>> result;
		result.resize(data.rows());

		// 最初の項は1で埋める
		for (int i = 0; i < data.rows(); i++)
		{
			result[i].push_back(1);
		}

		// degree=1のみをとりあえず想定(それ以外は現状考えない)
		// [[a,b],[c,b]]の場合は
		// [[1,a,b],[1,c,d]]となるだけ
		for (int i = 0; i < data.rows(); i++)
		{
			Eigen::MatrixXd rowData = data.row(i);

			for (int d = 0; d < rowData.cols(); d++)
			{
				result[i].push_back(rowData(0,d));
			}
		}

		return result;
	}

	void SetDegree(const size_t degree) { m_degree = degree; }

private:
	size_t m_degree;
};
