#pragma once
#include <Siv3D.hpp>

namespace TerakoyaRenderer
{
	class Primitive;

	class Mesh
	{
	public:
		std::weak_ptr<Primitive> CreatePrimitive();

		Array<std::shared_ptr<Primitive>> GetPrimitives() const;

	protected:
		Array<std::shared_ptr<Primitive>> m_primitives;
	};
}
