#include "Vertex.h"

namespace TerakoyaRenderer
{
	Vertex::Vertex(float x, float y, float z)
		: m_position(Vec3{x,y,z})
	{
	}

	Vertex::Vertex(Vec3 pos)
		: m_position(pos)
	{
	}
}
