#pragma once
#include <Siv3D.hpp>

namespace TerakoyaRenderer
{
	class ImageHolder
	{
	public:
		// 画像の取得
		std::weak_ptr<Image> GetImage(const size_t index) const;

	protected:
		// 予め確保できるように
		void ResizeHolder(const size_t size);

		// 画像の設定
		void SetImage(const String path, const size_t index);

	protected:
		Array<std::shared_ptr<Image>> m_images;

	private:
		friend class GLTFLoader;
	};
}
