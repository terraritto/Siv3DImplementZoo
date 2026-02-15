#include "Primitive.h"
#include "Vertex.h"

namespace TerakoyaRenderer
{
	Primitive::~Primitive()
	{
		m_vertices.clear();
	}

	void Primitive::AddVertex(float x, float y, float z)
	{
		std::shared_ptr<Vertex> vertex = std::make_shared<Vertex>(x, y, z);
		m_vertices.push_back(vertex);
	}

	void Primitive::AddVertex(std::shared_ptr<Vertex> vertex)
	{
		m_vertices.push_back(vertex);
	}

	void TerakoyaRenderer::Primitive::AddIndex(unsigned int index)
	{
		m_indices.push_back(index);
	}

	void Primitive::SetMaterialIndex(int index)
	{
		m_materialIndex = index;
	}

	const Array<std::shared_ptr<Vertex>>& Primitive::GetVertices() const
	{
		return m_vertices;
	}

	const Array<unsigned int>& Primitive::GetIndices() const
	{
		return m_indices;
	}

	const int Primitive::GetMaterialIndex() const
	{
		return m_materialIndex;
	}
}
