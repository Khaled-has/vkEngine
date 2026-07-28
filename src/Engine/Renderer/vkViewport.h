#pragma once

#include "RenderGraph.h"

#include "Viewport.h"

namespace Render
{
	class vkViewport : public Viewport
	{
	public:
		vkViewport(ViewportInfo mInfo) : Viewport(mInfo) {}

		virtual void Create() override;
		virtual void Destroy() override;

		virtual ImTextureID getImageView() override;

	private:
		RenderGraph mRenderGraph;
	};
}