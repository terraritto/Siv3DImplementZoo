#pragma once
#include <Siv3D.hpp>
#include <ThirdParty/Catch2/catch.hpp>

namespace TerakoyaRenderer
{
	class ResourceHolder;

	class ResourceBase
	{
	public:
		unsigned int GetResourceIndex() const { return m_resourceIndex; }
		size_t GetResourcesize() const { return m_resourceSize; }

		// リソースの取得
		template<class T>
		T* GetResource() const { return static_cast<T*>(m_resource); }

		// リソースの設定
		void CopyResource(void* src, unsigned int size)
		{
			if (m_resourceSize < size)
			{
				// もしリソースサイズが超えてる場合はおかしいのでエラーで落とす
				REQUIRE(false);
			}
			memcpy(m_resource, src, size);
		}

	protected:
		void* m_resource; // メモリアドレス
		size_t m_resourceSize; // リソースのサイズ
		unsigned int m_resourceIndex; // リソースID

		friend class ResourceHolder;
	};
}
