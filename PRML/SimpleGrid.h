#pragma once
#include <Siv3D.hpp>

class SimpleGrid
{
public:
	SimpleGrid();

	void Draw();

	void SetXGridRegion(Vec2 region) { m_xGridRegion = region; }
	void SetYGridRegion(Vec2 region) { m_yGridRegion = region; }
	void SetGridStep(Vec2 step) { m_gridStep = step; }

	[[nodiscard]]
	size_t GetWidth() const { return m_gridArea.w; }

	[[nodiscard]]
	size_t GetHeight() const { return m_gridArea.h; }

	[[nodiscard]]
	const Vec2 GetLeftCenter() const { return m_gridArea.leftCenter(); }

	[[nodiscard]]
	const Vector2D<double>& GetXGridRegion() const { return m_xGridRegion; }

	[[nodiscard]]
	const Vector2D<double>& GetYGridRegion() const { return m_yGridRegion; }

	[[nodiscard]]
	const Vector2D<double>& GetGridDelta() const { return m_gridDelta; }

	void SetIsDrawGrid(bool isDraw) { m_isDrawGrid = isDraw; }

protected:
	void UpdateInternal();

protected:
	Rect m_gridArea; // gridの描画エリア

	Vector2D<double> m_xGridRegion; // x方向のgrid分割比率
	Vector2D<double> m_yGridRegion; // y方向のgrid分割比率
	Vector2D<double> m_gridDelta;	

	Vector2D<double> m_gridStep;

	double m_gridAxisLine, m_gridLine;

	bool m_isDrawGrid;
};
