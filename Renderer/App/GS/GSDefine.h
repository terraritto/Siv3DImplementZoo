#pragma once
#include <Siv3D.hpp>

namespace TerakoyaRenderer::GS
{
	struct GSData
	{
		Array<Vec4> m_positions;
		Array<Mat4x4> m_scales;
		Array<Mat4x4> m_rotations;
		Array<double> m_opacities;
		Array<Vec4> m_dcs;
		Array<Array<Vec4>> m_shCoefficients;
	};

	struct GSCalculateData
	{
		Array<Vec4> m_us;
		Array<Vec4> m_viewPos;
		Array<Mat3x3> m_sigmaMatrices;
		Array<Vec4> m_colors;

		void Clear()
		{
			m_us.clear();
			m_viewPos.clear();
			m_sigmaMatrices.clear();
			m_colors.clear();
		}
	};
}
