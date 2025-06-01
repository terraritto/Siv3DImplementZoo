#pragma once
#include <Siv3D.hpp>

namespace TerakoyaRenderer
{
	class SceneNode
	{
	public:
		// Childrenの取得
		std::weak_ptr<SceneNode> AddChildren();
		const Array<std::shared_ptr<SceneNode>>& GetChildren();

		void SetTranslation(Vec3 trans);
		void SetRotation(Quaternion rotate);
		void SetMatrix(Mat4x4 matrix);

		void SetMeshIndex(int index);

		void SetParent(std::weak_ptr<SceneNode> parent) { m_parent = parent; }

		int GetMeshIndex() const { return m_meshIndex; }
		Mat4x4 GetMatrix() const { return m_matrix; }

	protected:
		Array<std::shared_ptr<SceneNode>> m_children;
		std::weak_ptr<SceneNode> m_parent;
		int m_meshIndex;
		Mat4x4 m_matrix = Mat4x4::Identity();
	};
}
