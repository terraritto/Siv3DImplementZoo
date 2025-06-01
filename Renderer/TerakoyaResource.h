#pragma once

#include <Siv3D.hpp>

namespace TerakoyaRenderer
{
	struct Vertex
	{
		Vec3 m_position;
	};

	struct Index
	{
		int m_index;
	};

	struct Mesh
	{
		Array<Vertex> m_vertices;
		Array<Index> m_indices;
	};
}
