#include "SimpleGrid.h"

SimpleGrid::SimpleGrid()
	: m_gridArea({0, 0, 800, 600})
	, m_xGridRegion({ -7.0, 7.0 })
	, m_yGridRegion({-1.0, 1.0})
	, m_gridDelta()
	, m_gridStep({0.6, 0.1})
	, m_gridAxisLine(2.0)
	, m_gridLine(0.3)
	, m_isDrawGrid(true)
{
	UpdateInternal();
}

void SimpleGrid::Draw()
{
	UpdateInternal();

	if (m_isDrawGrid)
	{
		for (int32 x = static_cast<int32>(m_xGridRegion.x / m_gridStep.x); x <= static_cast<int32>(m_xGridRegion.y / m_gridStep.x); ++x)
		{
			const double xAxis = (m_gridArea.x + (x * m_gridStep.x - m_xGridRegion.x) / m_gridDelta.x);
			const double thickness = ((x == 0) ? m_gridAxisLine : m_gridLine);
			RectF{ Arg::topCenter(xAxis, m_gridArea.y), thickness, static_cast<double>(m_gridArea.h) }.draw(ColorF{ 0.11 });
		}

		for (int32 y = static_cast<int32>(m_yGridRegion.x / m_gridStep.y); y <= static_cast<int32>(m_yGridRegion.y / m_gridStep.y); ++y)
		{
			const double yAxis = (m_gridArea.y + (y * m_gridStep.y - m_yGridRegion.x) / m_gridDelta.y);
			const double thickness = ((y == 0) ? m_gridAxisLine : m_gridLine);
			RectF{ Arg::leftCenter(m_gridArea.x, yAxis), static_cast<double>(m_gridArea.w), thickness }.draw(ColorF{ 0.11 });
		}
	}
}

void SimpleGrid::UpdateInternal()
{
	m_gridDelta.x = (m_xGridRegion.y - m_xGridRegion.x) / m_gridArea.w;
	m_gridDelta.y = (m_yGridRegion.y - m_yGridRegion.x) / m_gridArea.h;
}
