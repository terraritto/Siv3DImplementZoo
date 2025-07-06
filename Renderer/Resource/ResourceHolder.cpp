#include "ResourceHolder.h"
#include "ResourceBase.h"

namespace TerakoyaRenderer
{
	ResourceHolder::ResourceHolder()
	{
		// Allocatorの用意
		m_memoryAllocator = std::make_unique<Allocator<bool>>();

		// リソースの格納場所を用意
		m_resourceList.resize(RESOURCE_MAX);
	}
	ResourceHolder::~ResourceHolder()
	{
		for (std::weak_ptr<ResourceBase> resource : m_resourceList)
		{
			if (auto p = resource.lock(); p != nullptr)
			{
				// リソースがまだある場合は解放をしておく
				m_memoryAllocator->deallocate(p->GetResource<bool>(), p->m_resourceSize);
			}
		}

		// リソース関係を終了させる
		m_resourceList.clear();
		m_memoryAllocator.reset();
	}

	std::weak_ptr<ResourceBase> TerakoyaRenderer::ResourceHolder::GetResource()
	{
		unsigned int index = GetCurrentResourceIndex();

		//リソースの設定
		std::shared_ptr<ResourceBase> resource = std::make_shared<ResourceBase>();
		resource->m_resourceIndex = index;

		// メモリの確保
		resource->m_resource = m_memoryAllocator->allocate(resource->GetResourcesize());

		// 保持
		m_resourceList[index] = resource;
		return resource;
	}

	void ResourceHolder::ReleaseResource(std::weak_ptr<ResourceBase> resource)
	{
		if (auto p = resource.lock(); p != nullptr)
		{
			// 解放
			unsigned int index = p->GetResourceIndex();
			m_memoryAllocator->deallocate(p->GetResource<bool>(), p->GetResourcesize());
			m_resourceList[index].reset();

			// 解放したリストに入れておく
			m_freeIndexList.push_back(index);
		}
	}

	unsigned int ResourceHolder::GetCurrentResourceIndex()
	{
		if (RESOURCE_MAX <= m_resourceNum)
		{
			if (m_freeIndexList.isEmpty() == false)
			{
				// もし解放されているリソースがあればそれにアクセス
				int index = m_freeIndexList.back();
				m_freeIndexList.pop_back();
				return index;
			}
			else
			{
				// リソース確保上限となっているのでエラーで落としておく
				assert(false);
			}
		}

		// まだ確保可能
		return m_resourceNum++;
	}
}
