#pragma once
#include <Siv3D.hpp>
#include <Eigen/Dense>

class SimpleGrid;

class FunctionBase
{
public:
	enum class DrawType
	{
		Line,
		Circle,
		LineCircle,
		Hist,
		Scatter,
		ScatterHeatMap,
		None
	};

	FunctionBase(std::shared_ptr<SimpleGrid> grid);
	FunctionBase() = default;

	void ConstructLine();
	void CustomConstructLine(Array<double> data);
	void ConstructCircle(int sample = 100);
	void ConstructLineCircle(Array<double> data);
	void ConstructHistogram(Eigen::MatrixXd data, int bins);
	void ConstructScatter(Eigen::MatrixXd data);
	void Draw(Color color = Palette::Red, Color colorSecond = Palette::Blue);

	[[nodiscard]]
	const Array<double> GetTrainX() const { Array<double> temp; for (auto& data : m_valueList) { temp.push_back(data.x); } return temp; }

	[[nodiscard]]
	const Array<double> GetTrainY() const { Array<double> temp; for (auto& data : m_valueList) { temp.push_back(data.y); } return temp; }

protected:
	virtual double Calculate(const double) { return 0.0; };

	void Clear();

protected:
	std::weak_ptr<SimpleGrid> m_grid;
	DrawType m_type;

	size_t m_thickness;

	Array<Vec3> m_valueList;
	LineString m_lineList;
	Array<Circle> m_circleList;
	Array<RectF> m_rectList;
};
