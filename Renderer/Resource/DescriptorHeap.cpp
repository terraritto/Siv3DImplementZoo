#include "DescriptorHeap.h"

namespace TerakoyaRenderer
{
	DescriptorHeap::DescriptorHeap(DescriptorHeapDesc desc)
		: m_desc(desc)
	{
		// サイズ分を確保(中身はまだない状態)
		m_descriptorList.resize(m_desc.m_numDescriptors);
	}

	unsigned int DescriptorHeap::GetCurrentDescriptorIndex()
	{
		if (m_desc.m_numDescriptors <= m_descriptorNum)
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
		return m_descriptorNum++;
	}

	std::shared_ptr<DescriptorHandle> BufferDescriptorHeap::CreateDescriptor(BufferDescriptorDesc& bufferDesc)
	{
		unsigned int index = GetCurrentDescriptorIndex();

		// Descriptorの作成と登録
		std::unique_ptr<Descriptor> descriptor = std::make_unique<Descriptor>();
		descriptor->m_location = bufferDesc.m_location;
		descriptor->m_memoryOffset = bufferDesc.m_memoryOffset;
		descriptor->m_size = bufferDesc.m_size;
		m_descriptorList[index] = std::move(descriptor);

		// 返す用のHandleを作成する(RootParam用)
		std::shared_ptr<DescriptorHandle> handle = std::make_shared<DescriptorHandle>();
		handle->m_heap = shared_from_this();
		handle->m_index = index;
		return handle;
	}
}
