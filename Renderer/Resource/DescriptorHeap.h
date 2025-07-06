#pragma once
#include <Siv3D.hpp>
#include "ResourceDefine.h"
#include "Descriptor.h"

namespace TerakoyaRenderer
{
	class DescriptorHeap : public std::enable_shared_from_this<DescriptorHeap>
	{
	public:
		DescriptorHeap(DescriptorHeapDesc desc);

		// Descriptor Heapの型
		virtual DescirptorHeapType GetType() const { return DescirptorHeapType::InValid; }

	protected:
		// ディスクリプタのIndexの取得
		unsigned int GetCurrentDescriptorIndex();

	protected:
		unsigned int m_descriptorNum = 0;
		Array<unsigned int> m_freeIndexList;
		Array<std::unique_ptr<Descriptor>> m_descriptorList;
		DescriptorHeapDesc m_desc;
	};

	// Buffer関係のHeap
	class BufferDescriptorHeap : public DescriptorHeap
	{
	public:
		using DescriptorHeap::DescriptorHeap;

		// Buffer用のDescriptorを生成
		std::shared_ptr<DescriptorHandle> CreateDescriptor(BufferDescriptorDesc& bufferDesc);

		// DescriptorHeap override
		virtual DescirptorHeapType GetType() const override { return DescirptorHeapType::Resource; }
	};
}
