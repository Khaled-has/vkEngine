#include "VK_Device.h"

#include "config.h"

#define VMA_IMPLEMENTATION
#define VMA_STATIC_VULKAN_FUNCTIONS 0
#define VMA_DYNAMIC_VULKAN_FUNCTIONS 0
#include <vk_mem_alloc.h>

std::string GetDeviceType(VkPhysicalDeviceType pType);
VkFormat FindDepthFormat(VkPhysicalDevice Device);
static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
	VkDebugUtilsMessageSeverityFlagBitsEXT Severity,
	VkDebugUtilsMessageTypeFlagsEXT Type,
	const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
	void* pUserData);

bool VK_Device::Initialize(std::function<void(VkSurfaceKHR* pSurface, VkInstance mInstance)> CreateSurface)
{
	CreateInstance();
#if defined  (_DEBUG) && !(ANDROID)
	CreateDebugMessenger();
#endif
	CreateSurface(&m_pSurface, m_pInstance);
	EnumeratePhysicalDevices();
	CreateDevice();
	CreateAllocator();

	return true;
}

void VK_Device::Destroy()
{
	// # : Destroy VMA allocator
	vmaDestroyAllocator(m_pAllocator);

	// # : Destroy device
	vkDestroyDevice(m_pDevice, NULL);

	// # : Destroy surface
	vkDestroySurfaceKHR(m_pInstance, m_pSurface, NULL);

	// # : Destroy debug messenger utils
	vkDestroyDebugUtilsMessengerEXT(m_pInstance, m_pDebugMessenger, NULL);

	// # : Destroy instance
	vkDestroyInstance(m_pInstance, NULL);
}

void VK_Device::CreateInstance()
{
	// # Initialize volk
	CHECK_VK_RES(volkInitialize(), "volkInitialize");

	// # Enumerate instance version
	CHECK_VK_RES(vkEnumerateInstanceVersion(&m_pInstanceVersion), "vkEnumerateInstanceVersion");

	// # Application info
	VkApplicationInfo appInfo = {
		.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
		.pApplicationName = "vkEngine",
		.applicationVersion = VK_MAKE_API_VERSION(0, 1, 0, 0),
		.pEngineName = "vkEngine",
		.engineVersion = VK_MAKE_API_VERSION(0, 1, 0, 0),
		.apiVersion = m_pInstanceVersion
	};
	
	// # Layers & Extensions
	std::vector<const char*> layers = {
#if defined (_DEBUG) && !(ANDROID)
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

#if defined (_DEBUG) && !(ANDROID)
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

	CHECK_VK_RES(vkCreateInstance(&instInfo, NULL, &m_pInstance), "vkCreateInstance");
	// # volk: load instance
	volkLoadInstance(m_pInstance);
}

void VK_Device::CreateDebugMessenger()
{
	VkDebugUtilsMessengerCreateInfoEXT MessengerCreateInfo = {
			.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
			.pNext = NULL,

			.messageSeverity =
			VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,

			.messageType =
			VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,

			.pfnUserCallback = &DebugCallback,
			.pUserData = NULL
	};

	if (!vkCreateDebugUtilsMessengerEXT)
	{
		LOG_ERROR("Cannot find address of vkCreateDebugUtilsMessengerKHR");
		exit(1);
	}

	CHECK_VK_RES(vkCreateDebugUtilsMessengerEXT(m_pInstance, &MessengerCreateInfo, NULL, &m_pDebugMessenger), "vkCreateDebugUtilsMessenger");
}

void VK_Device::EnumeratePhysicalDevices()
{
	LOG_TRACE("\n====================================\n");
	// # Enumerate physical devices
	uint32_t PhysDevCount = 0;
	CHECK_VK_RES(vkEnumeratePhysicalDevices(m_pInstance, &PhysDevCount, NULL), "vkEnumeratePhysicalDevices");

	m_pPhysDevices.resize(PhysDevCount);
	LOG_INFO("Physical devices count: {0}", PhysDevCount);

	std::vector<VkPhysicalDevice> Devices(PhysDevCount);

	CHECK_VK_RES(vkEnumeratePhysicalDevices(m_pInstance, &PhysDevCount, Devices.data()), "vkEnumeratePhysicalDevices");

	for (uint32_t i = 0; i < PhysDevCount; i++)
	{
		VkPhysicalDevice PhyDev = Devices[i];

		m_pPhysDevices[i].m_pPhysDev = PhyDev;

		// # Device properties
		vkGetPhysicalDeviceProperties(PhyDev, &m_pPhysDevices[i].m_devProps);
		LOG_INFO("Device name: {0}", m_pPhysDevices[i].m_devProps.deviceName);
		LOG_INFO("Device type: {0}", GetDeviceType(m_pPhysDevices[i].m_devProps.deviceType));

		// # Device API version
		uint32_t apiV = m_pPhysDevices[i].m_devProps.apiVersion;
		m_pPhysDevices[i].m_apiVersion.Variant = VK_API_VERSION_VARIANT(apiV);
		m_pPhysDevices[i].m_apiVersion.Major = VK_API_VERSION_MAJOR(apiV);
		m_pPhysDevices[i].m_apiVersion.Minor = VK_API_VERSION_MINOR(apiV);
		m_pPhysDevices[i].m_apiVersion.Patch = VK_API_VERSION_PATCH(apiV);
		LOG_INFO("API version supported: {0}.{1}.{2}", VK_API_VERSION_MAJOR(apiV), VK_API_VERSION_MINOR(apiV), VK_API_VERSION_PATCH(apiV));

		// # Device extensions
		uint32_t extenCount = 0;
		vkEnumerateDeviceExtensionProperties(PhyDev, NULL, &extenCount, NULL);
		m_pPhysDevices[i].m_extensions.resize(extenCount);
		vkEnumerateDeviceExtensionProperties(PhyDev, NULL, &extenCount, m_pPhysDevices[i].m_extensions.data());

		LOG_TRACE("======> Physical device ( {0} ) extensions supported <======", i);
		for (auto& ext : m_pPhysDevices[i].m_extensions)
		{
			LOG_INFO("	{0}", ext.extensionName);
		}
		LOG_TRACE("==================================");

		// # Device queue families properties
		uint32_t NumQFamilies = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(PhyDev, &NumQFamilies, NULL);
		ASSERT(NumQFamilies >= 1, "Queue families = 0");

		m_pPhysDevices[i].m_qFamilyProps.resize(NumQFamilies);
		m_pPhysDevices[i].m_qSupportsPresent.resize(NumQFamilies);

		vkGetPhysicalDeviceQueueFamilyProperties(PhyDev, &NumQFamilies, m_pPhysDevices[i].m_qFamilyProps.data());

		for (uint32_t q = 0; q < NumQFamilies; q++)
		{
			const VkQueueFamilyProperties& QFamilyProp = m_pPhysDevices[i].m_qFamilyProps[q];

			LOG_INFO("	Family {0} Num queues: {1} ", q, QFamilyProp.queueCount);
			VkQueueFlags Flags = QFamilyProp.queueFlags;
			LOG_INFO("	GFX {0}, Compute {1}, Transfer {2}, Sparse binding {3}",
				(Flags & VK_QUEUE_GRAPHICS_BIT) ? "Yes" : "No",
				(Flags & VK_QUEUE_COMPUTE_BIT) ? "Yes" : "No",
				(Flags & VK_QUEUE_TRANSFER_BIT) ? "Yes" : "No",
				(Flags & VK_QUEUE_SPARSE_BINDING_BIT) ? "Yes" : "No"
			);

			CHECK_VK_RES(vkGetPhysicalDeviceSurfaceSupportKHR(PhyDev, q, m_pSurface, &(m_pPhysDevices[i].m_qSupportsPresent[q])), "vkGetPhysicalDeviceSurfaceSupportKHR");
			LOG_INFO("	 This Queue Support Present: {0}", m_pPhysDevices[i].m_qSupportsPresent[q] ? "Yes" : "No");
		}

		// # Device surface formats
		uint32_t NumFormats = 0;
		CHECK_VK_RES(vkGetPhysicalDeviceSurfaceFormatsKHR(PhyDev, m_pSurface, &NumFormats, NULL), "vkGetPhysicalDeviceSurfaceFormats");
		ASSERT(NumFormats > 0, "Num surface formats = 0");

		m_pPhysDevices[i].m_surfaceFormats.resize(NumFormats);

		CHECK_VK_RES(vkGetPhysicalDeviceSurfaceFormatsKHR(PhyDev, m_pSurface, &NumFormats, m_pPhysDevices[i].m_surfaceFormats.data()), "vkGetPhysicalDeviceSurfaceFormatsKHR");

		//LOG_TRACE("======> Physical device ( {0} ) surface formats <======", i);
		//for (auto& fo : m_pPhysDevices[i].m_surfaceFormats)
		//{
		//	LOG_INFO("	{0}", fo.format);
		//}
		//LOG_TRACE("==================================");
		
		// # Device surface capabilities
		CHECK_VK_RES(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(PhyDev, m_pSurface, &(m_pPhysDevices[i].m_surfaceCaps)), "vkGetPhysicalDeviceSurfaceCapabilitiesKHR");
		
		// # Device present modes
		uint32_t NumPresentModes = 0;
		CHECK_VK_RES(vkGetPhysicalDeviceSurfacePresentModesKHR(PhyDev, m_pSurface, &NumPresentModes, NULL), "vkGetPhysicalDeviceSurfacePresentModesKHR");
		ASSERT(NumPresentModes > 0, "Num present modes = 0");
		
		m_pPhysDevices[i].m_presentModes.resize(NumPresentModes);
		
		CHECK_VK_RES(vkGetPhysicalDeviceSurfacePresentModesKHR(PhyDev, m_pSurface, &NumPresentModes, m_pPhysDevices[i].m_presentModes.data()), "vkGetPhysicalDeviceSurfacePresentModesKHR");
		
		LOG_INFO("Number of presentation modes {0}", NumPresentModes);
		
		// # Device memory properties
		vkGetPhysicalDeviceMemoryProperties(PhyDev, &(m_pPhysDevices[i].m_memProps));
		
		// # Device features
		vkGetPhysicalDeviceFeatures(PhyDev, &(m_pPhysDevices[i].m_features));
		
		// # Device depth format
		m_pPhysDevices[i].m_depthFormat = FindDepthFormat(PhyDev);
	}

	// # Select physical device
	for (uint32_t i = 0; i < m_pPhysDevices.size(); i++)
	{
		for (uint32_t j = 0; j < m_pPhysDevices[i].m_qFamilyProps.size(); j++)
		{
			const VkQueueFamilyProperties& QFamilyProp = m_pPhysDevices[i].m_qFamilyProps[j];

			if ((QFamilyProp.queueFlags & VK_QUEUE_GRAPHICS_BIT) && ((bool)m_pPhysDevices[i].m_qSupportsPresent[j] == VK_TRUE))
			{
				m_selectedPhysDevIndex = i;

				if (m_pPhysDevices[i].m_devProps.deviceType != VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU)
				{
					LOG_INFO(
						"Using GFX device {0}",
						i
					);
				}
				else if (i <= m_pPhysDevices.size() - 1)
				{
					LOG_INFO(
						"Using GFX device {0}",
						i
					);
				}
			}
		}
	}
}

void VK_Device::CreateAllocator()
{
	// # VMA functions
	VmaVulkanFunctions vmaVulkanFunctions;

	// # VMA allocator create info
	VmaAllocatorCreateInfo allocatorInfo = {
		.flags = VMA_ALLOCATOR_CREATE_EXT_MEMORY_BUDGET_BIT,
		.physicalDevice = getSelectedPhysDevice().m_pPhysDev,
		.device = m_pDevice,
		.instance = m_pInstance,
		.vulkanApiVersion = m_pInstanceVersion,
	};

	CHECK_VK_RES(vmaImportVulkanFunctionsFromVolk(&allocatorInfo, &vmaVulkanFunctions), "vmaImportVulkanFunctionsFromVolk");

	allocatorInfo.pVulkanFunctions = &vmaVulkanFunctions;

	// # VmaAllocator creation
	CHECK_VK_RES(vmaCreateAllocator(&allocatorInfo, &m_pAllocator), "vmaCreateAllocator");
}

std::vector<const char*> VK_Device::getDeviceExtensions()
{
	std::vector<const char*> result = {
		VK_KHR_SWAPCHAIN_EXTENSION_NAME,
		VK_KHR_SHADER_DRAW_PARAMETERS_EXTENSION_NAME
	};

	VK_PhysDeviceProps& PhyDev = m_pPhysDevices[m_selectedPhysDevIndex];

	bool DeviceSupportsDynamicRendering = PhyDev.IsExtensionSupported(VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME);

	bool Instance_is_1_3_or_more = (PhyDev.m_apiVersion.Major > 1) || (PhyDev.m_apiVersion.Minor >= 3);

	if (Instance_is_1_3_or_more && DeviceSupportsDynamicRendering)
	{
		LOG_INFO("The Vulkan instance and device support dynamic rendering as a core feature");
		m_pPhysDevices[m_selectedPhysDevIndex].m_pIsDynamicSupported = true;
	}
	else if (PhyDev.m_apiVersion.Minor == 2)
	{
		if (DeviceSupportsDynamicRendering)
		{
			result.push_back(VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME);
			m_pPhysDevices[m_selectedPhysDevIndex].m_pIsDynamicSupported = true;
		}
		else
		{
			LOG_WARN("The system doesn't support dynamic rendering");
			m_pPhysDevices[m_selectedPhysDevIndex].m_pIsDynamicSupported = false;
		}
	}
	else {
		LOG_WARN("The system doesn't support dynamic rendering");
		m_pPhysDevices[m_selectedPhysDevIndex].m_pIsDynamicSupported = false;
	}

	return result;
}

void VK_Device::CreateDevice()
{
	// # Queues info
	const VK_PhysDeviceProps& PhyDev = getSelectedPhysDevice();

	uint32_t QFCount = PhyDev.m_qFamilyProps.size();
	const std::vector<VkQueueFamilyProperties> QFamilyProps = PhyDev.m_qFamilyProps;

	std::vector<VkDeviceQueueCreateInfo> queueFamilyInfos;
	int32_t graphicsQFamilyIndex = -1;
	int32_t computeQFamilyIndex = -1;
	int32_t transferQFamilyIndex = -1;

	uint32_t numProperties = 0;

	LOG_INFO("Found {0} queue families:", QFCount);

	for (uint32_t i = 0; i < QFCount; i++)
	{
		const VkQueueFamilyProperties& curProps = QFamilyProps[i];

		bool validQueue = false;

		// # 1: Graphics queue
		if (((curProps.queueFlags & VK_QUEUE_GRAPHICS_BIT) == VK_QUEUE_GRAPHICS_BIT) && graphicsQFamilyIndex == -1)
		{
			graphicsQFamilyIndex = i;
			validQueue = true;
		}

		// # 2: Compute queue
		if (
			((curProps.queueFlags & VK_QUEUE_COMPUTE_BIT) == VK_QUEUE_COMPUTE_BIT) && computeQFamilyIndex == -1 &&
			graphicsQFamilyIndex != i
			)
		{
			computeQFamilyIndex = i;
			validQueue = true;
		}

		// # 3: Transfer queue
		if (
			((curProps.queueFlags & VK_QUEUE_TRANSFER_BIT) == VK_QUEUE_TRANSFER_BIT) && transferQFamilyIndex == -1 &&
			graphicsQFamilyIndex != i && computeQFamilyIndex != i
			)
		{
			transferQFamilyIndex = i;
			validQueue = true;
		}

		if (!validQueue)
		{
			continue;
		}

		// # Add the new queue
		VkDeviceQueueCreateInfo qCreateInfo = {
			.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
			.queueFamilyIndex = i,
			.queueCount = curProps.queueCount,
		};

		queueFamilyInfos.push_back(qCreateInfo);
		numProperties += curProps.queueCount;
	}

	std::vector<float> queueProperties(numProperties);
	float* currentPriority = queueProperties.data();

	for (uint32_t i = 0; i < queueFamilyInfos.size(); i++)
	{
		queueFamilyInfos[i].pQueuePriorities = currentPriority;

		for (uint32_t j = 0; j < QFamilyProps[queueFamilyInfos[i].queueFamilyIndex].queueCount; j++)
		{
			*currentPriority++ = 1.0f;
		}
	}

	// # Create queues
	ASSERT(graphicsQFamilyIndex != -1, "Missing Vulkan queue graphics");

	m_pQueues.mGraphics = new VK_Queue(graphicsQFamilyIndex, 0);
	m_pQueues.mCompute = computeQFamilyIndex == -1 ? m_pQueues.mGraphics : new VK_Queue(computeQFamilyIndex, 0);
	m_pQueues.mTransfer = transferQFamilyIndex == -1 ? m_pQueues.mGraphics : new VK_Queue(transferQFamilyIndex, 0);
	m_pQueues.mPresent = m_pQueues.mGraphics;

	if (computeQFamilyIndex == -1) LOG_INFO("Use the graphics queue is a compute queue also");
	if (transferQFamilyIndex == -1) LOG_INFO("Use the graphics queue is a transfer queue also");

	// # Device extensions support
	const std::vector<const char*> extensions = getDeviceExtensions();

	// # Device features
	if (PhyDev.m_features.geometryShader == VK_FALSE)
	{
		LOG_WARN("The geometry shader is not supported");
	}
	if (PhyDev.m_features.tessellationShader == VK_FALSE)
	{
		LOG_WARN("The tessellation shader is not supported");
	}

	VkPhysicalDeviceFeatures DeviceFeature = { 0 };
	DeviceFeature.geometryShader = VK_TRUE;
	DeviceFeature.tessellationShader = VK_TRUE;

	if (!PhyDev.m_pIsDynamicSupported)
	{
		LOG_ERROR("Dynamic rendering is not supported: this engine only support dynamic rendering");
		exit(1);
	}

	VkPhysicalDeviceDynamicRenderingFeaturesKHR DynamicRenderingFeature = {
		.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES_KHR,
		.dynamicRendering = VK_TRUE
	};

	// # Device creation
	VkDeviceCreateInfo DeviceCreateInfo = {
		.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
		.pNext = &DynamicRenderingFeature,
		.queueCreateInfoCount = (uint32_t)queueFamilyInfos.size(),
		.pQueueCreateInfos = queueFamilyInfos.data(),
		.enabledLayerCount = 0,
		.ppEnabledLayerNames = NULL,
		.enabledExtensionCount = (uint32_t)extensions.size(),
		.ppEnabledExtensionNames = extensions.data(),
		.pEnabledFeatures = &DeviceFeature
	};

	CHECK_VK_RES(vkCreateDevice(PhyDev.m_pPhysDev, &DeviceCreateInfo, NULL, &m_pDevice), "vkCreateDevice");
	// # volk: load device
	volkLoadDevice(m_pDevice);

	// # Initialize queues
	m_pQueues.mGraphics->Initialize();
	m_pQueues.mCompute->Initialize();
	m_pQueues.mTransfer->Initialize();
}

std::string GetDeviceType(VkPhysicalDeviceType pType)
{
	std::string result = "";

	if ((pType & VK_PHYSICAL_DEVICE_TYPE_OTHER) == VK_PHYSICAL_DEVICE_TYPE_OTHER)
		result += "Other\t";

	if ((pType & VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU) == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU)
		result += "Integrated GPU\t";

	if ((pType & VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
		result += "Discrete GPU\t";

	if ((pType & VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU) == VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU)
		result += "Virtual GPU\t";

	if ((pType & VK_PHYSICAL_DEVICE_TYPE_CPU) == VK_PHYSICAL_DEVICE_TYPE_CPU)
		result += "CPU\t";

	if ((pType & VK_PHYSICAL_DEVICE_TYPE_MAX_ENUM) == VK_PHYSICAL_DEVICE_TYPE_MAX_ENUM)
		result += "Max Enum\t";

	return result;
}

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

	LOG_ERROR("Failed to find supported format!");
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

	LOG_INFO("Device depth format: {0} {1} {2}",
		DepthFormat == VK_FORMAT_D32_SFLOAT ? "VK_FORMAT_D32_SFLOAT" : "",
		DepthFormat == VK_FORMAT_D32_SFLOAT_S8_UINT ? "VK_FORMAT_D32_SFLOAT_S8_UINT" : "",
		DepthFormat == VK_FORMAT_D24_UNORM_S8_UINT ? "VK_FORMAT_D24_UNORM_S8_UINT" : ""
	);

	return DepthFormat;
}

const char* GetDebugType(VkDebugUtilsMessageTypeFlagsEXT Type)
{

	switch (Type) {
	case VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT:
		return "General";

	case VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT:
		return "Validation";

	case VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT:
		return "Performance";

#ifdef _WIN64 // doesn't work on my Linux for some reason
	case VK_DEBUG_UTILS_MESSAGE_TYPE_DEVICE_ADDRESS_BINDING_BIT_EXT:
		return "Device address binding";
#endif

	default:
		LOG_ERROR("Invalid type code {0}\n", static_cast<uint32_t>(Type));
		exit(1);
	}

	return "NO SUCH TYPE!";
}

const char* GetDebugSeverityStr(VkDebugUtilsMessageSeverityFlagBitsEXT Severity)
{

	switch (Severity) {
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
		return "Verbose";

	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
		return "Info";

	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
		return "Warning";

	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
		return "Error";

	default:
		LOG_ERROR("Invalid severity code");
		exit(1);
	}

	return "NO SUCH SEVERITY!";
}

static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
	VkDebugUtilsMessageSeverityFlagBitsEXT Severity,
	VkDebugUtilsMessageTypeFlagsEXT Type,
	const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
	void* pUserData)
{
	std::string pT = GetDebugSeverityStr(Severity);
	if (pT == "Error")
	{
		LOG_ERROR("Debug callback: {0} \n		Severity {1}\n			Type {2}\n		   Object ",
			pCallbackData->pMessage, GetDebugSeverityStr(Severity), GetDebugType(Type)
		);
	}
	else
	{
		LOG_TRACE("Debug callback: {0} \n		Severity {1}\n			Type {2}\n		   Object ",
			pCallbackData->pMessage, GetDebugSeverityStr(Severity), GetDebugType(Type)
		);
	}


	for (uint32_t i = 0; i < pCallbackData->objectCount; i++) {
#ifdef _WIN32
		if (pT == "Error")
		{
			LOG_ERROR("{0}", pCallbackData->pObjects[i].objectHandle);
		}
		else
		{
			LOG_TRACE("{0}", pCallbackData->pObjects[i].objectHandle);
		}
#else
		if (std::string(GetDebugSeverityStr(Severity)) == "Error")
		{
			LOG_ERROR("{0}", pCallbackData->pObjects[i].objectHandle);
		}
		else
		{
			LOG_TRACE("{0}", pCallbackData->pObjects[i].objectHandle);
		}
#endif
	}

	return VK_FALSE;  // The calling function should not be aborted
}