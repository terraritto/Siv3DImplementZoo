#pragma once
#include <Siv3D.hpp>
#include "../App/RenderAppBase.h"

namespace TerakoyaRenderer
{
	class Loader
	{
	public:
		virtual bool Load(String path, std::weak_ptr<RenderAppBase> app) = 0;
	};
}
