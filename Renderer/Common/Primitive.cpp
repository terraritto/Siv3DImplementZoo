#include "Primitive.h"
#include "Vertex.h"

namespace TerakoyaRenderer
{
	void Primitive::AddVertex(float x, float y, float z)
	{
		std::shared_ptr<Vertex> vertex = std::make_shared<Vertex>(x, y, z);
		m_vertices.push_back(vertex);
	}

	void TerakoyaRenderer::Primitive::AddIndex(unsigned int index)
	{
		m_indices.push_back(index);
	}
	const Array<std::shared_ptr<Vertex>>& Primitive::GetVertices() const
	{
		return m_vertices;
	}
	const Array<unsigned int>& Primitive::GetIndices() const
	{
		return m_indices;
	}
}
