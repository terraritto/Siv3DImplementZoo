#include "Scene.h"
#include "SceneNode.h"

namespace TerakoyaRenderer
{
	std::weak_ptr<SceneNode> Scene::AddRoot()
	{
		std::shared_ptr<SceneNode> node = std::make_shared<SceneNode>();
		m_roots.push_back(node);
		return node;
	}
	std::weak_ptr<SceneNode> Scene::GetRoot(int index)
	{
		return m_roots[index];
	}
}
