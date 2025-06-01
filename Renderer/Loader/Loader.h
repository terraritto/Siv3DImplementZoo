#pragma once
#include <Siv3D.hpp>

namespace TerakoyaRenderer
{
	class Loader
	{
	public:
		virtual bool Load(String path) = 0;
	};
}
