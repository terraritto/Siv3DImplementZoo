#pragma once
#include "RenderAppBase.h"

namespace TerakoyaRenderer
{
	class GltfTestApp : public RenderAppBase
	{
	public:
		using RenderAppBase::RenderAppBase;

	public:
		virtual void Prepare() override;
		virtual void Render() override;
	};
}
