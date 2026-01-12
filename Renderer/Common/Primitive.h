#pragma once
#include <Siv3D.hpp>

namespace TerakoyaRenderer
{
	class Vertex;

	class Primitive
	{
	public:
		void AddVertex(float x, float y, float z);
		void AddVertex(std::shared_ptr<Vertex> vertex);
		void AddIndex(unsigned int index);

		const Array<std::shared_ptr<Vertex>>& GetVertices() const;
		const Array<unsigned int>& GetIndices() const;

	protected:
		Array<std::shared_ptr<Vertex>> m_vertices;
		Array<unsigned int> m_indices;
	};
}
