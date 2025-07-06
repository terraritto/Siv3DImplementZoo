#pragma once
#include <Siv3D.hpp>

namespace TerakoyaRenderer
{
	class ResourceBase;

	// 確保可能なリソース数
	constexpr unsigned int RESOURCE_MAX = 65536;

	class ResourceHolder
	{
	public:
		// constructor
		ResourceHolder();

		// destructor
		~ResourceHolder();

		// リソースの取得
		std::weak_ptr<ResourceBase> GetResource();

		void ReleaseResource(std::weak_ptr<ResourceBase> resource);

	private:
		// リソースのIndexの取得
		unsigned int GetCurrentResourceIndex();

	protected:
		// リソース
		unsigned int m_resourceNum = 0;
		Array<std::shared_ptr<ResourceBase>> m_resourceList;
		Array<unsigned int> m_freeIndexList;

		// メモリ確保用のAllocator
		std::unique_ptr<Allocator<bool>> m_memoryAllocator;
	};
}
