#pragma once
# include <Siv3D.hpp>
# include <Eigen/Dense>

// @ grid関係 start
// Gridを生成する
Eigen::MatrixXd MakeSquareGrid(int size, float min, float max); // 2D
Eigen::MatrixXd MakeGrid1D(int size, float min, float max); // 1D

// XYのデータとZのデータからXYZの行列を生成
Eigen::MatrixXd MakeGridXYZ(Eigen::MatrixXd XY, Eigen::MatrixXd Z);

// XYZのデータからZ値の値で抽出
Eigen::MatrixXd ExtractDataFromZ(Eigen::MatrixXd XYZ, double min = 0.0, double max = 1.0);

// 正規分布をAxis分で生成
Eigen::MatrixXd MakeNormalDistributionAxis(int size, double mean = 1.0, double sigma = 2.0, int axisNum = 2);
// @ grid関係 end

// @ convert関係 start
// MatrixをArrayに変換
Array<double> ConvertArrayFromMatrix(Eigen::MatrixXd X);

// ArrayをMatrixに変換(1D)
Eigen::MatrixXd ConvertMatrix1DFromArray(Array<double> X);

// 数値をMatrixに変換
template<class T>
Eigen::MatrixXd ConvertMatrixFromNumeric(const T value)
{
	Eigen::MatrixXd result(1, 1);
	result(0, 0) = static_cast<double>(value);
	return result;
}
// @ convert関係 end

// @ 判定関係 start
// Matrixはdoubleか？
bool IsScalar(Eigen::MatrixXd x);
// @ 判定関係 end

// digammaの実装
double digamma(double x);

// multivariate_normalの実装
double mvn_norm_pdf(const Eigen::MatrixXd& input,
					const Eigen::MatrixXd& mean,
					const Eigen::MatrixXd& covariance);
