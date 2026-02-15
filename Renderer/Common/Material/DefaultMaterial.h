#pragma once
#include <Siv3D.hpp>

namespace TerakoyaRenderer
{
	class DefaultMaterial
	{
	public:
		std::weak_ptr<Image> GetBaseColorTexture() const { return m_baseColorTexture; }

	protected:
		void SetBaseColorTexture(std::weak_ptr<Image> texture) { m_baseColorTexture = texture; }

	protected:
		std::weak_ptr<Image> m_baseColorTexture;

	private:
		friend class GLTFLoader;
	};
}
