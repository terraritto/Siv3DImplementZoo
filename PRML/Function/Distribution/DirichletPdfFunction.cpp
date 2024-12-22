#include "DirichletPdfFunction.h"

DirichletPdfFunction::DirichletPdfFunction(std::shared_ptr<SimpleGrid> grid, Eigen::MatrixXd alpha)
	: FunctionBase(grid)
	, m_alpha(alpha)
{
}
