#pragma once
#include <Siv3D.hpp>

namespace TerakoyaRenderer
{
	class DepthBuffer
	{
	public:
		DepthBuffer(int width, int height, double init = 0.0)
		{
			m_data.resize(height);
			for (auto& data : m_data) { data.resize(width, std::numeric_limits<double>::lowest()); }
		}

	public:
		Array<Array<double>> m_data;
	};
}
