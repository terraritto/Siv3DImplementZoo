#include "CategoricalPdfFunction.h"

CategoricalPdfFunction::CategoricalPdfFunction(std::shared_ptr<SimpleGrid> grid)
	: FunctionBase(grid)
{
}

CategoricalPdfFunction::CategoricalPdfFunction(std::shared_ptr<SimpleGrid> grid, std::weak_ptr<DirichletPdfFunction> dirichlet)
	: FunctionBase(grid)
{
	if (auto pDirichlet = dirichlet.lock())
	{
		m_dirichlet = std::make_unique<DirichletPdfFunction>(*pDirichlet.get());
	}
}

void CategoricalPdfFunction::Fit(Eigen::MatrixXd data)
{

	int rows = data.rows();
	Eigen::MatrixXd result(rows, 1);

	for (int i = 0; i < rows; i++)
	{
		result(i, 0) = m_dirichlet ? data.row(i).sum() : data.row(i).mean();
	}

	if (m_dirichlet)
	{
		m_dirichlet->m_alpha += result;
		return;
	}

	m_mu = result;
}

double CategoricalPdfFunction::Calculate(const double x)
{
    return 0.0;
}
