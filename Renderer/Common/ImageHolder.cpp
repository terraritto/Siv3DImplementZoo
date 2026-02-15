#include "ImageHolder.h"

namespace TerakoyaRenderer
{
	void ImageHolder::ResizeHolder(const size_t size)
	{
		this->m_images.resize(size);
	}

	void ImageHolder::SetImage(const String path, const size_t index)
	{
		assert(0 <= index);
		auto image = std::make_shared<Image>(path);
		this->m_images[index] = image;
	}

	std::weak_ptr<Image> ImageHolder::GetImage(const size_t index) const
	{
		assert(0 <= index && index < m_images.size());
		return this->m_images[index];
	}
}
