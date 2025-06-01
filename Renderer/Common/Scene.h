#pragma once
#include <Siv3D.hpp>

namespace TerakoyaRenderer
{
	class SceneNode;

	class Scene
	{
	public:
		std::weak_ptr<SceneNode> AddRoot();
		std::weak_ptr<SceneNode> GetRoot(int index = 0);

	protected:
		Array<std::shared_ptr<SceneNode>> m_roots;
	};
}
