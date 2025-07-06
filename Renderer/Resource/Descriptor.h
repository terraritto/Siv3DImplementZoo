#pragma once
#include <Siv3D.hpp>
#include <ThirdParty/Catch2/catch.hpp>

namespace TerakoyaRenderer
{
	class ResourceBase;
	class DescriptorHeap;

	// Shader側で参照されるDescriptor
	class Descriptor
	{
	public:
		std::weak_ptr<ResourceBase> m_location; // 位置
		int m_memoryOffset; // メモリの開始位置用オフセット
		int m_size; // メモリのサイズ
	};

	// 位置参照用のHandle
	class DescriptorHandle
	{
	public:
		std::weak_ptr<DescriptorHeap> m_heap;
		unsigned int m_index;
	};
}
