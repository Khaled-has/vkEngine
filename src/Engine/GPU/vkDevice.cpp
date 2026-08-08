#include "vkDevice.h"
#include "vkContext.h"
#include "vkLog.h"

namespace GPU
{

	void vkDevice::Create(const vkDeviceInfo& mInfo)
	{
		CheckContext("Create Device");
		m_pTargetPhysDevice = mInfo.pPhysDevice;

		const auto& device = GetCurrentContext()->getDeviceRes();

		// # Check
		if (device.mInitialized)
		{
			GPU_LOG_WARN("vkGPU: You can't create more than one vkDevice in the context");
			return;
		}

		// # Queues info
		auto prop = m_pTargetPhysDevice->getInfo();

		uint32_t QFCount = prop.mFamilyProps.size();
		const std::vector<VkQueueFamilyProperties> QFamilyProps = prop.mFamilyProps;

		std::vector<VkDeviceQueueCreateInfo> queueFamilyInfos;
		int32_t graphicsQFamilyIndex = -1;
		int32_t computeQFamilyIndex = -1;
		int32_t transferQFamilyIndex = -1;

		uint32_t numProperties = 0;

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

		// # Device features
		auto devFeatures = mInfo.pPhysDevice->getInfo().mFeatures;
		if (devFeatures.geometryShader == VK_FALSE)
		{
			GPU_LOG_WARN("vkGPU: The geometry shader is not supported");
		}
		if (devFeatures.tessellationShader == VK_FALSE)
		{
			GPU_LOG_WARN("vkGPU: The tessellation shader is not supported");
		}

		VkPhysicalDeviceFeatures DeviceFeature = { 0 };
		DeviceFeature.geometryShader = VK_TRUE;
		DeviceFeature.tessellationShader = VK_TRUE;

		if (!mInfo.pPhysDevice->getInfo().mIsDynamicSupported)
		{
			GPU_LOG_ERROR("vkGPU: Dynamic rendering is not supported: this engine only support dynamic rendering");
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
			.enabledExtensionCount = (uint32_t)mInfo.mExtensions.size(),
			.ppEnabledExtensionNames = mInfo.mExtensions.data(),
			.pEnabledFeatures = &DeviceFeature
		};

		GPU_CHECK(vkCreateDevice(m_pTargetPhysDevice->getHandle(), &DeviceCreateInfo, NULL, &m_Device), "vkCreateDevice");
		// # volk: load device
		volkLoadDevice(m_Device);

		// # Create queues
		GPU_ASSERT(graphicsQFamilyIndex != -1, "Missing Vulkan queue graphics");

		// # Graphics
		m_DeviceQueues.Graphics = new vkQueue;
		m_DeviceQueues.Graphics->CreateQueue(graphicsQFamilyIndex, 0);
		// # Transfer
		if (transferQFamilyIndex == -1)
			m_DeviceQueues.Transfer = m_DeviceQueues.Graphics;
		else
		{
			m_DeviceQueues.Transfer = new vkQueue;
			m_DeviceQueues.Transfer->CreateQueue(transferQFamilyIndex, 0);
		}
		// # Compute
		if (computeQFamilyIndex == -1)
			m_DeviceQueues.Compute = m_DeviceQueues.Graphics;
		else
		{
			m_DeviceQueues.Compute = new vkQueue;
			m_DeviceQueues.Compute->CreateQueue(computeQFamilyIndex, 0);
		}
		// # Present
		m_DeviceQueues.Present = m_DeviceQueues.Graphics;

		std::string deviceName = mInfo.pPhysDevice->getInfo().mDevProps.deviceName;
		if (computeQFamilyIndex == -1) GPU_LOG_INFO("vkGPU: {0}: Use the graphics queue as a compute queue also", deviceName);
		if (transferQFamilyIndex == -1) GPU_LOG_INFO("vkGPU: {0}: Use the graphics queue as a transfer queue also", deviceName);
	}
}