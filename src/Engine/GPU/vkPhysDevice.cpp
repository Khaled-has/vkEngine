#include "vkPhysDevice.h"
#include "vkContext.h"
#include "vkLog.h"

namespace GPU
{

	VkFormat FindSupportedFormat(VkPhysicalDevice Device, const std::vector<VkFormat>& Candidates,
		VkImageTiling Tiling, VkFormatFeatureFlags Features)
	{
		for (int i = 0; i < Candidates.size(); i++)
		{
			VkFormat Format = Candidates[i];
			VkFormatProperties Props;
			vkGetPhysicalDeviceFormatProperties(Device, Format, &Props);

			if ((Tiling == VK_IMAGE_TILING_LINEAR) &&
				(Props.linearTilingFeatures & Features) == Features)
			{
				return Format;
			}
			else if ((Tiling == VK_IMAGE_TILING_OPTIMAL) &&
				(Props.optimalTilingFeatures & Features) == Features)
			{
				return Format;
			}
		}

		GPU_LOG_ERROR("vkGPU: Failed to find supported format!");
		exit(1);
	}

	VkFormat FindDepthFormat(VkPhysicalDevice Device)
	{
		std::vector<VkFormat> Candidates = {
			VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT,
			VK_FORMAT_D24_UNORM_S8_UINT
		};

		VkFormat DepthFormat = FindSupportedFormat(
			Device, Candidates, VK_IMAGE_TILING_OPTIMAL,
			VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
		);

		return DepthFormat;
	}

    uint32_t GetPhysDevicesCount()
    {
        CheckContext("Call GetPhysDevicesCount() func");
        uint32_t res = 0;
        vkEnumeratePhysicalDevices(GetCurrentContext()->getInstanceRes().mInstance, &res, NULL);
        return res;
    }

    void EnumeratePhysDevices(std::span<vkPhysDevice> pPhysDevices)
    {
        CheckContext("Call EnumeratePhysDevices() func");

        // # Check
		uint32_t dCount = GetPhysDevicesCount();
        if (pPhysDevices.size() != dCount)
        {
            GPU_LOG_ERROR("vkGPU: You can't set number of PhysDevices large than the GetPhysDevicesCount() func get you in the EnumeratePhysDevices() func");
            exit(1);
        }

		// # Enumerate the physical devices
		std::vector<VkPhysicalDevice> devices(dCount);
		vkEnumeratePhysicalDevices(GetCurrentContext()->getInstanceRes().mInstance, &dCount, devices.data());

        // # Create the physical devices
        for (uint32_t i = 0; i < dCount; i++)
        {
			pPhysDevices[i].Create(devices[i]);
        }
    }

	void vkPhysDevice::Create(VkPhysicalDevice mPhysDevice)
	{
		m_PhysDevice = mPhysDevice;

		// # Device properties
		vkGetPhysicalDeviceProperties(mPhysDevice, &m_Info.mDevProps);

		// # Device extensions
		uint32_t extenCount = 0;
		vkEnumerateDeviceExtensionProperties(mPhysDevice, NULL, &extenCount, NULL);
		m_Info.mExtensions.resize(extenCount);
		vkEnumerateDeviceExtensionProperties(mPhysDevice, NULL, &extenCount, m_Info.mExtensions.data());

		// # Device queue families properties
		uint32_t NumQFamilies = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(mPhysDevice, &NumQFamilies, NULL);
		GPU_ASSERT(NumQFamilies >= 1, "Queue families = 0");

		m_Info.mFamilyProps.resize(NumQFamilies);
		m_Info.mSupportsPresent.resize(NumQFamilies);

		vkGetPhysicalDeviceQueueFamilyProperties(mPhysDevice, &NumQFamilies, m_Info.mFamilyProps.data());

		for (uint32_t q = 0; q < NumQFamilies; q++)
		{
			const VkQueueFamilyProperties& QFamilyProp = m_Info.mFamilyProps[q];
		}

		// # Device surface formats
		VkSurfaceKHR surface = GetCurrentContext()->getSurfaceRes().mSurface;
		uint32_t NumFormats = 0;
		GPU_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(mPhysDevice, surface, &NumFormats, NULL), "vkGetPhysicalDeviceSurfaceFormats");
		GPU_ASSERT(NumFormats > 0, "Num surface formats = 0");

		m_Info.mSurfaceFormats.resize(NumFormats);

		GPU_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(mPhysDevice, surface, &NumFormats, m_Info.mSurfaceFormats.data()), "vkGetPhysicalDeviceSurfaceFormatsKHR");

		// # Device surface capabilities
		GPU_CHECK(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(mPhysDevice, surface, &(m_Info.mSurfaceCaps)), "vkGetPhysicalDeviceSurfaceCapabilitiesKHR");

		// # Device present modes
		uint32_t NumPresentModes = 0;
		GPU_CHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(mPhysDevice, surface, &NumPresentModes, NULL), "vkGetPhysicalDeviceSurfacePresentModesKHR");
		GPU_ASSERT(NumPresentModes > 0, "Num present modes = 0");

		m_Info.mPresentModes.resize(NumPresentModes);

		GPU_CHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(mPhysDevice, surface, &NumPresentModes, m_Info.mPresentModes.data()), "vkGetPhysicalDeviceSurfacePresentModesKHR");

		// # Device memory properties
		vkGetPhysicalDeviceMemoryProperties(mPhysDevice, &(m_Info.mMemProps));

		// # Device features
		vkGetPhysicalDeviceFeatures(mPhysDevice, &(m_Info.mFeatures));

		// # Device depth format
		m_Info.mDepthFormat = FindDepthFormat(mPhysDevice);
	}
}