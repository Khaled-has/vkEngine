#ifndef VK_DEVICE_H
#define VK_DEVICE_H

#include <iostream>
#include <vector>
#include <functional>

#include "VK_Queue.h"
#include <vk_mem_alloc.h>

#include "VK_CmdBuf.h"

struct VK_PhysDeviceProps
{
	VkPhysicalDevice m_pPhysDev;
	VkPhysicalDeviceProperties m_devProps;
	std::vector<VkQueueFamilyProperties> m_qFamilyProps;
	std::vector<VkBool32> m_qSupportsPresent;
	std::vector<VkSurfaceFormatKHR> m_surfaceFormats;
	VkSurfaceCapabilitiesKHR m_surfaceCaps;
	VkPhysicalDeviceMemoryProperties m_memProps;
	std::vector<VkPresentModeKHR> m_presentModes;
	VkPhysicalDeviceFeatures m_features;
	VkFormat m_depthFormat;
	bool m_pIsDynamicSupported;
	struct {
		int Variant = 0;
		int Major = 0;
		int Minor = 0;
		int Patch = 0;
	} m_apiVersion;
	std::vector<VkExtensionProperties> m_extensions;

	inline bool IsExtensionSupported(const char* extension) const
	{
		std::string Ext = extension;

		for (uint32_t i = 0; i < m_extensions.size(); i++)
		{
			std::string e = m_extensions[i].extensionName;
			if (Ext == e)
			{
				return true;
			}
		}
		return false;
	}
};

class VK_Device
{
public:
	VK_Device() { pDeviceInstance = this; }


	bool Initialize();
	void Destroy();

	inline static VK_Device* Get() { return pDeviceInstance; }
	inline static void Set(VK_Device* pDevice) { pDeviceInstance = pDevice; }

	inline const VkInstance& getInstance() const { return m_pInstance; }
	inline const VkSurfaceKHR getSurface() const { return m_pSurface; }
	inline const VkDevice& getDevice() const { return m_pDevice; }
	inline const VK_PhysDeviceProps& getSelectedPhysDevice() const { return m_pPhysDevices[m_selectedPhysDevIndex]; }
	inline const VmaAllocator& getAllocator() const { return m_pAllocator; }
	inline const auto& getQueues() const { return m_pQueues; }

	inline uint32_t getInstanceVersion() const { return m_pInstanceVersion; }

	// # NOTE: This CmdBuf only used for simple operations & only in the main thread
	inline const VK_CmdPool& getCmdPool() { return m_CmdPool; }
	inline const VK_CmdBuf& getCmdBuf() { return m_SimpleOperationCmdBuf; }
private:
	inline static VK_Device* pDeviceInstance;

	// # Vulkan components
	VkInstance m_pInstance = VK_NULL_HANDLE;
	VkDebugUtilsMessengerEXT m_pDebugMessenger = VK_NULL_HANDLE;
	VkSurfaceKHR m_pSurface = VK_NULL_HANDLE;
	VkDevice m_pDevice = VK_NULL_HANDLE;
	VmaAllocator m_pAllocator = VK_NULL_HANDLE;

	std::vector<VK_PhysDeviceProps> m_pPhysDevices;
	uint32_t m_selectedPhysDevIndex = 0;

	struct {
		VK_Queue* mGraphics;
		VK_Queue* mCompute = nullptr;
		VK_Queue* mTransfer = nullptr;
		VK_Queue* mPresent = nullptr;
	} m_pQueues;

	uint32_t m_pInstanceVersion = 0;

	// # Used only for simple operations : ( NOTE: ONLY ON THE MAIN THREAD )
	VK_CmdPool m_CmdPool;
	VK_CmdBuf m_SimpleOperationCmdBuf;

	// # Create functions
	void CreateInstance();
	void CreateDebugMessenger();
	void EnumeratePhysicalDevices();
	void CreateDevice();
	void CreateAllocator();

	std::vector<const char*> getDeviceExtensions();

	void CreateSimpleOperationsCmdBuf();
	void DestroySimpleOperationsCmdBuf();
};

#endif