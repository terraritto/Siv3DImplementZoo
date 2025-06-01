#include "Mesh.h"
#include "Primitive.h"

namespace TerakoyaRenderer
{
	std::weak_ptr<Primitive> Mesh::CreatePrimitive()
	{
		std::shared_ptr<Primitive> primitive = std::make_shared<Primitive>();
		m_primitives.push_back(primitive);
		return primitive;
	}

	Array<std::shared_ptr<Primitive>> Mesh::GetPrimitives() const
	{
		return m_primitives;
	}
}
