﻿#pragma once
#include <Siv3D.hpp>

namespace TerakoyaRenderer
{
	class Vertex
	{
	public:
		Vertex(float x, float y, float z);
		Vertex(Vec3 pos);

		// 位置情報
		Vec3 m_position;
	};
}
