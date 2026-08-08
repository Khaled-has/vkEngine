#pragma once

#include "Renderer.h"
#include "vkGPU.h"

#include "vkViewport.h"

namespace Render
{
	class vkRenderer : public Renderer
	{
	public:
		vkRenderer() {}

		virtual void Initialize() override;
		virtual void Destroy() override;

		virtual void Rendering() override;

		virtual void AddViewportImpl(Viewport* pViewport) override;

	private:
		std::vector<GPU::vkPhysDevice> m_PhysDevices;
	};
}