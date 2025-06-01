#include "SceneNode.h"

namespace TerakoyaRenderer
{
	std::weak_ptr<SceneNode> SceneNode::AddChildren()
	{
		std::shared_ptr<SceneNode> node = std::make_shared<SceneNode>();
		m_children.push_back(node);
		return node;
	}

	const Array<std::shared_ptr<SceneNode>>& TerakoyaRenderer::SceneNode::GetChildren()
	{
		return m_children;
	}

	void SceneNode::SetTranslation(Vec3 trans)
	{
		m_matrix = m_matrix.translated(trans);
	}

	void SceneNode::SetRotation(Quaternion rotate)
	{
		m_matrix = m_matrix.rotated(rotate);
	}

	void SceneNode::SetMatrix(Mat4x4 matrix)
	{
		m_matrix = m_matrix * matrix;
	}

	void SceneNode::SetMeshIndex(int index)
	{
		m_meshIndex = index;
	}
}
