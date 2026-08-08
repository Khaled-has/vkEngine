#include "vkCreations.h"

#include <vector>

namespace GPU
{
	VkInstance CreateInstance(uint32_t mInstanceVersion, uint32_t mAppVersion, uint32_t mEngineVersion)
	{
		// # Application info
		VkApplicationInfo appInfo = {
			.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
			.pApplicationName = "vkEngine",
			.applicationVersion = mAppVersion,
			.pEngineName = "vkEngine",
			.engineVersion = mEngineVersion,
			.apiVersion = mInstanceVersion
		};

		// # Layers & Extensions
		std::vector<const char*> layers = {
	#if defined (WIN32) || (__linux__)
			"VK_LAYER_KHRONOS_validation"
	#endif
		};

		std::vector<const char*> extensions = {
			VK_KHR_SURFACE_EXTENSION_NAME,
	#ifdef WIN32
			"VK_KHR_win32_surface",
	#elif ANDROID
			"VK_KHR_android_surface",
	#elif __linux__
			"VK_KHR_xcb_surface",
	#endif

	#if defined (WIN32) || (__linux__)
			VK_EXT_DEBUG_UTILS_EXTENSION_NAME
	#endif
		};

		// # Create the instance
		VkInstanceCreateInfo instInfo = {
			.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
			.pNext = NULL,
			.flags = 0,
			.pApplicationInfo = &appInfo,
			.enabledLayerCount = (uint32_t)layers.size(),
			.ppEnabledLayerNames = layers.data(),
			.enabledExtensionCount = (uint32_t)extensions.size(),
			.ppEnabledExtensionNames = extensions.data()
		};

		VkInstance res;
		GPU_CHECK(vkCreateInstance(&instInfo, NULL, &res), "vkCreateInstance");

		return res;
	}
}