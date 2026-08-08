#ifndef VKPHYSDEVICE_H
#define VKPHYSDEVICE_H

#include "vkConfig.h"

namespace GPU
{
	class VKGPU_API vkPhysDevice
	{
	public:
		vkPhysDevice() {}
		void Create(VkPhysicalDevice mPhysDevice);

		const VkPhysicalDevice& getHandle() const { return m_PhysDevice; }

		inline const auto& getInfo() const { return m_Info; }

	private:
		VkPhysicalDevice m_PhysDevice = VK_NULL_HANDLE;

		struct {
			VkPhysicalDevice mPhysDev;
			VkPhysicalDeviceProperties mDevProps;
			std::vector<VkQueueFamilyProperties> mFamilyProps;
			std::vector<VkBool32> mSupportsPresent;
			std::vector<VkSurfaceFormatKHR> mSurfaceFormats;
			VkSurfaceCapabilitiesKHR mSurfaceCaps;
			VkPhysicalDeviceMemoryProperties mMemProps;
			std::vector<VkPresentModeKHR> mPresentModes;
			VkPhysicalDeviceFeatures mFeatures;
			VkFormat mDepthFormat;
			std::vector<VkExtensionProperties> mExtensions;
			bool mIsDynamicSupported;
		} m_Info;
	};

	VKGPU_API uint32_t GetPhysDevicesCount();
	VKGPU_API void EnumeratePhysDevices(std::span<vkPhysDevice> pPhysDevices);
}

#endif