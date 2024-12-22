#include "PRMLUtil.h"

Eigen::MatrixXd MakeSquareGrid(int size, float min, float max)
{
	Eigen::ArrayXf xLine = Eigen::ArrayXf::LinSpaced(size, min, max);
	Eigen::ArrayXf yLine = Eigen::ArrayXf::LinSpaced(size, min, max);

	Eigen::MatrixXd MeshGridData(size * size, 2);
	for (int i = 0; i < size; i++)
	{
		for (int j = 0; j < size; j++)
		{
			MeshGridData(i * size + j, 0) = xLine(j);
			MeshGridData(i * size + j, 1) = yLine(i);
		}
	}

	return MeshGridData;
}

Eigen::MatrixXd MakeGrid1D(int size, float min, float max)
{
	Eigen::ArrayXf xLine = Eigen::ArrayXf::LinSpaced(size, min, max);

	Eigen::MatrixXd MeshGridData(size, 1);
	for (int i = 0; i < size; i++)
	{
		MeshGridData(i, 0) = xLine(i);
	}

	return MeshGridData;
}

Eigen::MatrixXd MakeGridXYZ(Eigen::MatrixXd XY, Eigen::MatrixXd Z)
{
	Eigen::MatrixXd MeshGridResult(XY.rows(), 3);
	for (int i = 0; i < XY.rows(); i++)
	{
		MeshGridResult(i, 0) = XY(i, 0); // X
		MeshGridResult(i, 1) = XY(i, 1); // Y
		MeshGridResult(i, 2) = Z(i, 0);  // Z
	}

	return MeshGridResult;
}

Eigen::MatrixXd ExtractDataFromZ(Eigen::MatrixXd XYZ, double min, double max)
{
	Array<Vec3> extractData;

	assert(XYZ.cols() == 3);

	for (int i = 0; i < XYZ.rows(); i++)
	{
		double data = XYZ(i, 2);
		if (min <= data && data <= max)
		{
			extractData.push_back(Vec3{ XYZ(i,0), XYZ(i,1), XYZ(i,2) });
		}
	}

	Eigen::MatrixXd result(extractData.size(), 3);
	for (int i = 0; i < extractData.size(); i++)
	{
		result(i, 0) = extractData[i].x;
		result(i, 1) = extractData[i].y;
		result(i, 2) = extractData[i].z;
	}

	return result;
}

Eigen::MatrixXd MakeNormalDistributionAxis(int size, double mean, double sigma, int axisNum)
{
	std::random_device randomDevice;
	std::default_random_engine engine(randomDevice());
	std::normal_distribution<> normalDistribution = std::normal_distribution<>(mean, sigma);

	Eigen::MatrixXd data(size, axisNum);

	for (int i = 0; i < size; i++)
	{
		for (int j = 0; j < axisNum; j++)
		{
			data(i, j) = normalDistribution(randomDevice);
		}
	}

	return data;
}

Array<double> ConvertArrayFromMatrix(Eigen::MatrixXd X)
{
	assert(X.cols() == 1);

	size_t size = X.rows();

	Array<double> result(size, 0.0);
	for (int i = 0; i < X.rows(); i++)
	{
		result[i] = X(i, 0);
	}

	return result;
}

Eigen::MatrixXd ConvertMatrix1DFromArray(Array<double> X)
{
	Eigen::MatrixXd result(X.size(), 1);

	for (int i = 0; i < X.size(); i++)
	{
		result(i, 0) = X[i];
	}

	return result;
}

bool IsScalar(Eigen::MatrixXd x)
{
	return x.cols() == 1 && x.rows() == 1;
}

double digamma(double x)
{
	double c = 8.5;
	double d1 = -0.5772156649;
	double r;
	double s = 0.00001;
	double s3 = 0.08333333333;
	double s4 = 0.0083333333333;
	double s5 = 0.003968253968;
	double value;
	double y;
	//
	//  Check the input.
	//
	if (x <= 0.0)
	{
		value = 0.0;
		return value;
	}
	//
	//  Initialize.
	//
	y = x;
	value = 0.0;
	//
	//  Use approximation if argument <= S.
	//
	if (y <= s)
	{
		value = d1 - 1.0 / y;
		return value;
	}
	//
	//  Reduce to DIGAMA(X + N) where (X + N) >= C.
	//
	while (y < c)
	{
		value = value - 1.0 / y;
		y = y + 1.0;
	}
	//
	//  Use Stirling's (actually de Moivre's) expansion if argument > C.
	//
	r = 1.0 / y;
	value = value + log(y) - 0.5 * r;
	r = r * r;
	value = value - r * (s3 - r * (s4 - r * s5));

	return value;
}

double mvn_norm_pdf(const Eigen::MatrixXd& input, const Eigen::MatrixXd& mean, const Eigen::MatrixXd& covariance)
{
	static const double log_sqrt_2_pi = double(0.5 * std::log(2 * M_PI));

	typedef Eigen::LLT<Eigen::MatrixXd> Cholesky;
	Cholesky chol(covariance.template cast<double>());
	if (chol.info() != Eigen::Success) throw "Decomposition failed!";
	const typename Cholesky::Traits::MatrixL& L = chol.matrixL();

	double quad_form = (L.solve(input - mean)).squaredNorm();
	double det = L.determinant();

	return Exp(double(-input.rows()) * log_sqrt_2_pi - 0.5 * quad_form) / det;
}
