#ifndef VKCONTEXT_H
#define VKCONTEXT_H

#include "vkConfig.h"
#include "vkDevice.h"
#include "vkSwapChain.h"

namespace GPU
{

	struct vkVolkResources
	{
		bool mInitialized = false;
		PFN_vkGetInstanceProcAddr mInstanceProcAddr;
	};

	struct vkInstanceResources
	{
		bool mInitialized = false;
		VkInstance mInstance;
		uint32_t mInstanceVersion;
	};

	struct vkSurfaceResources
	{
		bool mInitialized = false;
		VkSurfaceKHR mSurface;
	};

	struct vkDeviceResources
	{
		bool mInitialized = false;
		std::unique_ptr<vkDevice> pDevice;
		vkDeviceQueues mDeviceQueues;
	};

	struct vkVmaResources
	{
		bool mInitialized = false;
		VmaAllocator mAllocator;
	};

	struct vkSwapChainResources
	{
		bool mInitialized = false;
		std::unique_ptr<vkSwapChain> pSwapChain;
	};

	class VKGPU_API vkContext
	{
	public:
		vkContext() {}

		inline const vkVolkResources& getInstanceProcAddrRes()  const { return m_VolkRes; }
		inline const vkInstanceResources& getInstanceRes()		const { return m_InstanceRes; }
		inline const vkSurfaceResources& getSurfaceRes()		const { return m_SurfaceRes; }
		inline const vkDeviceResources& getDeviceRes()			const { return m_DeviceRes; }
		inline const vkVmaResources& getVmaRes()				const { return m_VmaRes; }
		inline const vkSwapChainResources& getSwapChainRes()	const { return m_SwapChainRes; }

	private:
		friend void CreateContext(std::function<void(VkInstance, VkSurfaceKHR*)> mCrtSurface);

		friend vkDevice* CreateDevice(const vkDeviceInfo& mInfo);
		friend vkSwapChain* CreateSwapChain(bool mVSync);
	private:
		// # Context values
		vkVolkResources m_VolkRes;
		vkInstanceResources m_InstanceRes;
		vkSurfaceResources m_SurfaceRes;
		vkDeviceResources m_DeviceRes;
		vkVmaResources m_VmaRes;
		vkSwapChainResources m_SwapChainRes;
	};

	// # Create the context
	VKGPU_API void CreateContext(std::function<void(VkInstance, VkSurfaceKHR*)> mCrtSurface);
	// # Destroy the context
	VKGPU_API void DestroyContext();
	// # Call this if you need to use the context from another lib without create new one
	VKGPU_API void SetCurrentContext(vkContext* pCurrent);
	// # Get the context
	VKGPU_API const vkContext* GetCurrentContext();

	void CheckContext(const char* pName);

	// # vkDevice create function
	VKGPU_API vkDevice* CreateDevice(const vkDeviceInfo& mInfo);
	// # vkSwapChain create function
	VKGPU_API vkSwapChain* CreateSwapChain(bool mVSync);
}

#endif