#include "vkRenderer.h"
#include <imgui.h>
#include <imgui_impl_vulkan.h>

#include <Window.h>

namespace Render
{

	Renderer* Renderer::pInstance = nullptr;

	void vkRenderer::Initialize()
	{
		Renderer::pInstance = this;

		GPU::CreateContext(
			[](VkInstance mInstance, VkSurfaceKHR* pSurface) {
				Window::getInstance()->CreateVkSurface(mInstance, pSurface);
			}
		);

		// # Create physical device
		m_PhysDevices.resize(GPU::GetPhysDevicesCount());

		GPU::EnumeratePhysDevices(m_PhysDevices);

		// # Create logical device
		GPU::vkDeviceInfo deviceInfo = {
			.pPhysDevice = &m_PhysDevices[0]
		};

		GPU::CreateDevice(deviceInfo);
		GPU::CreateSwapChain(true);

		GPU::vkCmdPool pool;
		pool.Create(GPU::GetCurrentContext()->getDeviceRes().mDeviceQueues.Transfer->getQFamilyIndex());

		GPU::vkCmdBuf cmds[] = { GPU::vkCmdBuf() };
		pool.AllocCmdBufs(cmds);

		const GPU::vkContext* ctx = GPU::GetCurrentContext();
	}

	void vkRenderer::Destroy()
	{
		
	}

	void vkRenderer::Rendering()
	{
		
	}

	void vkRenderer::AddViewportImpl(Viewport* pViewport)
	{
		
	}
}