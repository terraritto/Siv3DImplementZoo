#include "FunctionBase.h"
#include "SimpleGrid.h"

FunctionBase::FunctionBase(std::shared_ptr<SimpleGrid> grid)
	: m_grid(grid)
	, m_type(DrawType::None)
	, m_thickness(3)
{
}

void FunctionBase::Draw(Color color, Color colorSecond)
{
	switch (m_type)
	{
	case DrawType::Line:
		m_lineList.draw(static_cast<double>(m_thickness), color);
		break;

	case DrawType::Circle:
	case DrawType::Scatter:
		for (auto& circle : m_circleList)
		{
			circle.draw(color);
		}
		break;

	case DrawType::LineCircle:
		{
			// Lineの描画
			m_lineList.draw(static_cast<double>(m_thickness), color);

			// Circleの描画
			for (auto& circle : m_circleList)
			{
				circle.draw(colorSecond);
			}
		}
		break;

	case DrawType::Hist:
		{
			for (auto& rect : m_rectList)
			{
				rect.draw(color);
			}
		}
		break;

	case DrawType::ScatterHeatMap:
	{
		double minValue = DBL_MAX; double maxValue = DBL_MIN;
		for (auto& data : m_valueList)
		{
			double value = data.z;
			minValue = value < minValue ? value : minValue;
			maxValue = maxValue < value ? value : maxValue;
		}

		for (int i = 0; i < m_valueList.size(); i++)
		{
			double value = m_valueList[i].z;
			double threshold = (value - minValue) / (maxValue - minValue);
			Color color = Colormap01(threshold, s3d::ColormapType::Heat);

			m_circleList[i].draw(color);
		}
	}
	break;

	default:
		break;
	}
}

void FunctionBase::Clear()
{
	m_valueList.clear();
	m_circleList.clear();
	m_lineList.clear();
	m_rectList.clear();
}

void FunctionBase::ConstructLine()
{
	m_type = DrawType::Line;
	Clear();

	auto pGrid = m_grid.lock();

	// gridから必要な情報を抽出
	const size_t N = pGrid->GetWidth();
	const Vec2 leftCenter = pGrid->GetLeftCenter();
	const double xStart = pGrid->GetXGridRegion().x;
	const double xStep = (pGrid->GetXGridRegion().y - pGrid->GetXGridRegion().x) / N;
	const double yStep = (pGrid->GetYGridRegion().y - pGrid->GetYGridRegion().x) / N * 2;//pGrid->GetGridDelta().y;

	// 関数処理の対応
	m_valueList.resize(N + 1);
	for (size_t i = 0; i < (N + 1); ++i)
	{
		const double x = xStart + xStep * i;
		m_valueList[i].x = x;
		m_valueList[i].y = Calculate(x);
	}

	// Lineの構築
	m_lineList.resize(m_valueList.size());
	for (size_t i = 0; i < m_valueList.size(); ++i)
	{
		// -を掛けるのはwindowの座標軸の仕様のため
		m_lineList[i] = leftCenter + Vec2{i, (m_valueList[i].y * (1.0 / -yStep))};
	}
}

void FunctionBase::CustomConstructLine(Array<double> data)
{
	m_type = DrawType::Line;
	Clear();

	auto pGrid = m_grid.lock();

	// gridから必要な情報を抽出
	const size_t N = pGrid->GetWidth() - 1;
	const double Height = static_cast<double>(pGrid->GetHeight());
	const Vec2 leftCenter = pGrid->GetLeftCenter();
	const double xStart = pGrid->GetXGridRegion().x;
	const double xStep = pGrid->GetGridDelta().x;
	const double yStep = (pGrid->GetYGridRegion().y - pGrid->GetYGridRegion().x);

	// 関数処理の対応
	m_valueList.resize(N + 1);
	for (size_t i = 0; i < (N + 1); ++i)
	{
		const double x = xStart + xStep * i;
		m_valueList[i].x = x;
		m_valueList[i].y = data[i];
	}

	// Lineの構築
	m_lineList.resize(m_valueList.size());
	for (size_t i = 0; i < m_valueList.size(); ++i)
	{
		// -を掛けるのはwindowの座標軸の仕様のため
		m_lineList[i] = leftCenter + Vec2{i, (-m_valueList[i].y / yStep) * Height};
	}
}

void FunctionBase::ConstructCircle(int sample)
{
	m_type = DrawType::Circle;
	Clear();

	auto pGrid = m_grid.lock();

	// gridから必要な情報を抽出
	const int N = static_cast<int>(pGrid->GetWidth());
	const Vec2 leftCenter = pGrid->GetLeftCenter();
	const double discreteStep = N / static_cast<double>(sample);
	const double xStart = pGrid->GetXGridRegion().x;
	const double xStep = (pGrid->GetXGridRegion().y - pGrid->GetXGridRegion().x) / sample;
	const double yStep = (pGrid->GetYGridRegion().y - pGrid->GetYGridRegion().x) / N * 2;

	// 関数処理の対応
	m_valueList.resize(sample + 1);
	for (size_t i = 0; i < (sample + 1); ++i)
	{
		const double x = xStart + xStep * i;
		m_valueList[i].x = x;
		m_valueList[i].y = Calculate(x);
	}

	// Lineの構築
	m_circleList.resize(m_valueList.size());
	for (size_t i = 0; i < m_valueList.size(); ++i)
	{
		// -を掛けるのはwindowの座標軸の仕様のため
		m_circleList[i] = { leftCenter + Vec2{i* discreteStep, (m_valueList[i].y * (1.0 / -yStep))}, 5 };
	}
}

void FunctionBase::ConstructLineCircle(Array<double> data)
{
	m_type = DrawType::LineCircle;
	Clear();

	auto pGrid = m_grid.lock();

	// gridから必要な情報を抽出
	const int N = static_cast<int>(pGrid->GetWidth());
	const double Height = static_cast<double>(pGrid->GetHeight());
	int sample = static_cast<int>(data.size()) - 1;
	const Vec2 leftCenter = pGrid->GetLeftCenter();
	const double discreteStep = N / static_cast<double>(sample);
	const double xStart = pGrid->GetXGridRegion().x;
	//const double xStep = pGrid->GetGridDelta().x * discreteStep;
	//const double yStep = pGrid->GetGridDelta().y;

	const double xStep = (pGrid->GetXGridRegion().y - pGrid->GetXGridRegion().x) / sample;
	//const double yStep = (pGrid->GetYGridRegion().y - pGrid->GetYGridRegion().x);
	const double yStep = (pGrid->GetYGridRegion().y - pGrid->GetYGridRegion().x) / N;

	// 関数処理の対応
	m_valueList.resize(sample + 1);
	for (size_t i = 0; i < (sample + 1); ++i)
	{
		const double x = xStart + xStep * i;
		m_valueList[i].x = x;
		m_valueList[i].y = data[i];
	}

	// Lineの構築
	m_circleList.resize(m_valueList.size());
	m_lineList.resize(m_valueList.size());

	Vec2 left = Vec2{ leftCenter.x, 0.0 };
	for (size_t i = 0; i < m_valueList.size(); ++i)
	{
		double y = Math::Smoothstep(pGrid->GetYGridRegion().x, pGrid->GetYGridRegion().y, m_valueList[i].y);
		y = 1.0 - y;
		y *= Height;

		m_circleList[i] = { left + Vec2{i* discreteStep, y}, 5 };
		m_lineList[i] = left + Vec2{i* discreteStep, y};
			;//(m_valueList[i].y* (1.0 / -yStep)) / Height};
	}
}

void FunctionBase::ConstructHistogram(Eigen::MatrixXd data, int bins)
{
	m_type = DrawType::Hist;
	Clear();

	auto pGrid = m_grid.lock();

	// gridから必要な情報を抽出
	const int N = static_cast<int>(pGrid->GetWidth());
	const double Height = static_cast<double>(pGrid->GetHeight());
	int sample = static_cast<int>(bins) - 1;
	const Vec2 leftCenter = pGrid->GetLeftCenter();
	const double discreteStep = N / static_cast<double>(sample);
	const double xStart = pGrid->GetXGridRegion().x;
	const double xStep = pGrid->GetGridDelta().x * discreteStep;
	const double yStep = (pGrid->GetYGridRegion().y - pGrid->GetYGridRegion().x);

	// データをソートしてbinを用意
	Array<double> sampleData;
	Array<int> binList(bins, 0); Array<double> result(bins, 0.0);
	for (int i = 0; i < data.rows(); i++)
	{
		sampleData.push_back(data(i, 0));
	}
	sampleData.sort(); 

	// binを構築
	int binIndex = 0;
	const double binsStep = (pGrid->GetXGridRegion().y - xStart) / bins;
	for (double& dataSample : sampleData)
	{
		while (true)
		{
			double threshold = xStart + binsStep * (binIndex + 1);
			if (dataSample < threshold)
			{
				// 範囲内なら足す
				binList[binIndex]++;
				break;
			}
			else
			{
				// 範囲外なら次のbinへ移動
				binIndex++;

				if (binIndex >= binList.size()) {
					break;
				}
			}
		}

		// Listのサイズを超えてしまっていたら終了
		if (binIndex >= binList.size()) { break; }
	}

	// 結果を生成
	for (int i = 0; i < binList.size(); i++)
	{
		result[i] = binList[i] / static_cast<double>(sampleData.size());
	}

	// 関数処理の対応
	m_valueList.resize(sample + 1);
	for (size_t i = 0; i < (sample + 1); ++i)
	{
		const double x = xStart + xStep * i;
		m_valueList[i].x = x;
		m_valueList[i].y = result[i];
	}

	// Rectの構築
	m_rectList.resize(m_valueList.size());
	for (size_t i = 0; i < m_valueList.size(); ++i)
	{
		// -を掛けるのはwindowの座標軸の仕様のため
		m_rectList[i] = RectF{ leftCenter.x + i * discreteStep, leftCenter.y, discreteStep, (m_valueList[i].y / (-yStep)) * (Height / 2)};
	}
}

void FunctionBase::ConstructScatter(Eigen::MatrixXd data)
{
	bool isExistZ = data.cols() == 3;
	m_type = isExistZ ? DrawType::ScatterHeatMap : DrawType::Scatter;
	Clear();

	auto pGrid = m_grid.lock();

	// gridから必要な情報を抽出
	const int N = static_cast<int>(pGrid->GetWidth());
	const double Height = static_cast<double>(pGrid->GetHeight());
	int sample = static_cast<int>(data.rows()) - 1;
	const Vec2 leftCenter = pGrid->GetLeftCenter();
	const double discreteStep = N / static_cast<double>(sample);
	const double xStart = pGrid->GetXGridRegion().x;
	const double yStart = pGrid->GetYGridRegion().x;
	const double xStep = pGrid->GetGridDelta().x;
	const double yStep = (pGrid->GetYGridRegion().y - pGrid->GetYGridRegion().x);

	// 関数処理の対応
	m_valueList.resize(sample + 1);
	for (size_t i = 0; i < (sample + 1); ++i)
	{
		const double x = (data(i, 0) - xStart) / (pGrid->GetXGridRegion().y - xStart);
		const double y = (data(i, 1) - yStart) / (pGrid->GetYGridRegion().y - yStart);

		m_valueList[i].x = x;
		m_valueList[i].y = y;

		if (isExistZ) { m_valueList[i].z = data(i, 2); }
	}

	// Rectの構築
	m_circleList.resize(m_valueList.size());
	for (size_t i = 0; i < m_valueList.size(); ++i)
	{
		// -を掛けるのはwindowの座標軸の仕様のため
		Vec2 offset;
		offset.x = m_valueList[i].x * N;
		offset.y = -m_valueList[i].y * Height;
		Vec2 left = leftCenter; left.y = Height;
		m_circleList[i] = { left + offset, 5};
	}
}
