#ifndef VKDEVICE_H
#define VKDEVICE_H

#include "vkPhysDevice.h"
#include "vkQueue.h"

namespace GPU
{
	struct vkDeviceInfo
	{
		const vkPhysDevice* pPhysDevice;

		std::vector<const char*> mExtensions = /* Default EXT */ {
			VK_KHR_SWAPCHAIN_EXTENSION_NAME,
			VK_KHR_SHADER_DRAW_PARAMETERS_EXTENSION_NAME,
			VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME
		};
	};

	class VKGPU_API vkDevice
	{
	public:
		vkDevice() {}

		inline const VkDevice& getHandle()				const { return m_Device; }
		inline const vkPhysDevice& getPhysDevice()		const { return *m_pTargetPhysDevice; }
		inline const vkDeviceQueues& getDeviceQueues()	const { return m_DeviceQueues; }

	private:
		friend vkDevice* CreateDevice(const vkDeviceInfo& mInfo);

		void Create(const vkDeviceInfo& mInfo);
	
	private:
		VkDevice m_Device = VK_NULL_HANDLE;
		const vkPhysDevice* m_pTargetPhysDevice = NULL;
		vkDeviceQueues m_DeviceQueues;
	};
}

#endif