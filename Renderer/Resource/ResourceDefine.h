#pragma once
#include <Siv3D.hpp>

namespace TerakoyaRenderer
{
	class ResourceBase;

	// Descriptor Heapが管理する型
	enum class DescirptorHeapType
	{
		Resource,
		Sampler,
		RTV,
		DSV,
		InValid
	};

	// Descriptor Heap用のDesc
	struct DescriptorHeapDesc
	{
		unsigned int m_numDescriptors;
	};

	// 1次元BufferのDesc
	struct BufferDescriptorDesc
	{
		std::weak_ptr<ResourceBase> m_location; // 位置
		int m_memoryOffset; // メモリの開始位置用オフセット
		int m_size; // メモリのサイズ
	};

}
