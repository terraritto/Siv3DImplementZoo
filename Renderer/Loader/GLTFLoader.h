#pragma once
#include "Loader.h"

namespace TerakoyaRenderer
{
	class GLTFLoader : public Loader
	{
	public:
		virtual bool Load(String path, std::weak_ptr<RenderAppBase> app) override;
	};
}
