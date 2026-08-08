#include "vkContext.h"

// # Vulkan memory allocator
#define VMA_IMPLEMENTATION
#define VMA_STATIC_VULKAN_FUNCTIONS 0
#define VMA_DYNAMIC_VULKAN_FUNCTIONS 0
#include <vk_mem_alloc.h>

#include "vkCreations.h"

#include <iostream>

namespace GPU
{

	vkContext* vkCtx = NULL;

	void CreateContext(std::function<void(VkInstance, VkSurfaceKHR*)> mCrtSurface)
	{
		// # Check if context is loaded
		if (vkCtx)
		{
			GPU_LOG_WARN("vkGPU: The context is created before | Delete it if you want to create new one");
			return;
		}
		else
			vkCtx = new vkContext;
		

		// # Initialize volk
		if (vkCtx->m_VolkRes.mInitialized)
		{
			GPU_LOG_WARN("vkGPU: volk is already Initialized");
		}
		else
		{
			volkInitialize();
			vkCtx->m_VolkRes.mInstanceProcAddr = vkGetInstanceProcAddr;
			vkCtx->m_VolkRes.mInitialized = true;
		}

		// # Create instance
		if (vkCtx->m_InstanceRes.mInitialized)
		{
			GPU_LOG_WARN("vkGPU: The vulkan instance is already created");
		}
		else
		{
			// # Enumerate instance's version
			vkEnumerateInstanceVersion(&vkCtx->m_InstanceRes.mInstanceVersion);
			// # Create instance
			vkCtx->m_InstanceRes.mInstance = CreateInstance(
				vkCtx->m_InstanceRes.mInstanceVersion,
				VK_MAKE_API_VERSION(0, 1, 0, 0), // App version
				VK_MAKE_API_VERSION(0, 1, 0, 0)  // Engine version
			);

			vkCtx->m_InstanceRes.mInitialized = true;
			volkLoadInstance(vkCtx->m_InstanceRes.mInstance);
		}

		// # Create vulkan surface
		if (vkCtx->m_SurfaceRes.mInitialized)
		{
			GPU_LOG_WARN("vkGPU: The vulkan surface is already created");
		}
		else
		{
			mCrtSurface(vkCtx->m_InstanceRes.mInstance, &vkCtx->m_SurfaceRes.mSurface);
			vkCtx->m_SurfaceRes.mInitialized = true;
		}
		std::cout << "Created\n";
	}

	void SetCurrentContext(vkContext* pCurrent)
	{
		vkCtx = pCurrent;
	}

	const vkContext* GetCurrentContext()
	{
		return vkCtx;
	}

	void CheckContext(const char* pName)
	{
		if (vkCtx == nullptr)
		{
			GPU_LOG_ERROR("vkGPU: Create the context before you try to: {0}", pName);
			exit(1);
		}
	}

	vkDevice* CreateDevice(const vkDeviceInfo& mInfo)
	{
		auto& device = vkCtx->m_DeviceRes.pDevice;
		device = std::make_unique<vkDevice>();
		device->Create(mInfo);
		// # Device queues
		vkCtx->m_DeviceRes.mDeviceQueues = device->m_DeviceQueues;

		vkCtx->m_DeviceRes.mInitialized = true;

		{
			// # Initialize VMA
			// # VMA functions
			VmaVulkanFunctions vmaVulkanFunctions;

			// # VMA allocator create info
			VmaAllocatorCreateInfo allocatorInfo = {
				.flags = VMA_ALLOCATOR_CREATE_EXT_MEMORY_BUDGET_BIT,
				.physicalDevice = device->getPhysDevice().getHandle(),
				.device = device->getHandle(),
				.instance = vkCtx->m_InstanceRes.mInstance,
				.vulkanApiVersion = vkCtx->m_InstanceRes.mInstanceVersion,
			};

			GPU_CHECK(
				vmaImportVulkanFunctionsFromVolk(&allocatorInfo, &vmaVulkanFunctions), "vmaImportVulkanFunctionsFromVolk"
			);

			allocatorInfo.pVulkanFunctions = &vmaVulkanFunctions;

			// # VmaAllocator creation
			GPU_CHECK(vmaCreateAllocator(&allocatorInfo, &vkCtx->m_VmaRes.mAllocator), "vmaCreateAllocator");
			vkCtx->m_VmaRes.mInitialized = true;
		}

		return device.get();
	}

	vkSwapChain* CreateSwapChain(bool mVSync)
	{
		auto& swap = vkCtx->m_SwapChainRes.pSwapChain;
		swap = std::make_unique<vkSwapChain>();
		swap->Create(mVSync);
		vkCtx->m_SwapChainRes.mInitialized = true;

		return swap.get();
	}
}