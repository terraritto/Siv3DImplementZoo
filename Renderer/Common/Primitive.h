#pragma once
#include <Siv3D.hpp>

namespace TerakoyaRenderer
{
	class Vertex;

	class Primitive
	{
	public:
		~Primitive();

		void AddVertex(float x, float y, float z);
		void AddVertex(std::shared_ptr<Vertex> vertex);
		void AddIndex(unsigned int index);

		// setter
		void SetMaterialIndex(int index);

		// getter
		const Array<std::shared_ptr<Vertex>>& GetVertices() const;
		const Array<unsigned int>& GetIndices() const;
		const int GetMaterialIndex() const;

	protected:
		Array<std::shared_ptr<Vertex>> m_vertices;
		Array<unsigned int> m_indices;
		int m_materialIndex;
	};
}
