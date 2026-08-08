#include "vkSemaphore.h"
#include "vkContext.h"
#include "vkLog.h"

namespace GPU
{
	VkSemaphore CreateVkSemaphore()
	{
		VkSemaphoreCreateInfo CreateInfo = {
			.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
			.pNext = NULL,
			.flags = 0
		};

		VkSemaphore Semaphore;
		GPU_CHECK(vkCreateSemaphore(
			GetCurrentContext()->getDeviceRes().pDevice->getHandle(), &CreateInfo,
			NULL, &Semaphore
		), "vkCreateSemaphore");

		return Semaphore;
	}

	void vkSemaphoreManager::Create()
	{
		m_NumImages = GetCurrentContext()->getSwapChainRes().pSwapChain->getSwapImageCount();
		m_Semaphores.resize(m_NumImages);

		VkFenceCreateInfo fenceInfo = {
			.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
			.pNext = NULL,
			.flags = VK_FENCE_CREATE_SIGNALED_BIT
		};

		for (auto& Sem : m_Semaphores)
		{
			Sem.mImageAvailableSem = CreateVkSemaphore();
			Sem.mRenderFinishedSem = CreateVkSemaphore();

			GPU_CHECK(vkCreateFence(
				GetCurrentContext()->getDeviceRes().pDevice->getHandle(), &fenceInfo, NULL, &Sem.mInFlightFence
			), "vkCreateFence");
			Sem.mImageInFlightFence = VK_NULL_HANDLE;
		}
	}

	void vkSemaphoreManager::Destroy()
	{
		const VkDevice& device = GetCurrentContext()->getDeviceRes().pDevice->getHandle();

		for (auto& Sem : m_Semaphores)
		{
			vkDestroySemaphore(device, Sem.mImageAvailableSem, NULL);
			vkDestroySemaphore(device, Sem.mRenderFinishedSem, NULL);

			vkDestroyFence(device, Sem.mInFlightFence, NULL);
		}
	}

	VkResult vkSemaphoreManager::AcquireNextImage()
	{
		const VkDevice& device = GetCurrentContext()->getDeviceRes().pDevice->getHandle();
		const VkSwapchainKHR& swapChain = GetCurrentContext()->getSwapChainRes().pSwapChain->getHandle();
		vkWaitForFences(device, 1, &m_Semaphores[m_FrameIndex].mInFlightFence, VK_TRUE, UINT64_MAX);
		vkResetFences(device, 1, &m_Semaphores[m_FrameIndex].mInFlightFence);

		VkResult res = vkAcquireNextImageKHR(
			device, swapChain,
			UINT64_MAX, m_Semaphores[m_FrameIndex].mImageAvailableSem,
			VK_NULL_HANDLE, &m_AcquiredImageIndex
		);

		if ((m_Semaphores[m_AcquiredImageIndex].mImageInFlightFence != VK_NULL_HANDLE) &&
			(m_Semaphores[m_AcquiredImageIndex].mImageInFlightFence != m_Semaphores[m_FrameIndex].mInFlightFence))
		{
			vkWaitForFences(device, 1, &m_Semaphores[m_AcquiredImageIndex].mImageInFlightFence, VK_TRUE, UINT64_MAX);
		}

		m_Semaphores[m_AcquiredImageIndex].mImageInFlightFence = m_Semaphores[m_FrameIndex].mInFlightFence;

		if (res == VK_ERROR_OUT_OF_DATE_KHR || res == VK_SUBOPTIMAL_KHR)
		{
			return res;
		}

		if (res != VK_SUCCESS)
		{
			LOG_ERROR("Error: vkAcquiredNextImage");
		}

		return res;
	}

	void vkSemaphoreManager::SubmitAsync(std::span<const vkCmdBuf> mCmdBufs)
	{
		static VkPipelineStageFlags waitFlags[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

		std::vector<VkCommandBuffer> Cmds;
		for (uint32_t i = 0; i < mCmdBufs.size(); i++)
		{
			Cmds.push_back(mCmdBufs[i].getHandle());
		}

		VkSubmitInfo SubmitInfo = {
			.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
			.pNext = NULL,
			.waitSemaphoreCount = 1,
			.pWaitSemaphores = &m_Semaphores[m_FrameIndex].mImageAvailableSem,
			.pWaitDstStageMask = waitFlags,
			.commandBufferCount = (uint32_t)Cmds.size(),
			.pCommandBuffers = Cmds.data(),
			.signalSemaphoreCount = 1,
			.pSignalSemaphores = &m_Semaphores[m_FrameIndex].mRenderFinishedSem
		};

		GPU_CHECK(
			vkQueueSubmit(GetCurrentContext()->getDeviceRes().mDeviceQueues.Graphics->getHandle(), 1, &SubmitInfo, m_Semaphores[m_FrameIndex].mInFlightFence),
			"vkQueueSubmit"
		);
	}

	VkResult vkSemaphoreManager::Present(uint32_t mImageIndex)
	{
		VkPresentInfoKHR PresentInfo = {
			.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
			.pNext = NULL,
			.waitSemaphoreCount = 1,
			.pWaitSemaphores = &m_Semaphores[m_FrameIndex].mRenderFinishedSem,
			.swapchainCount = 1,
			.pSwapchains = &GetCurrentContext()->getSwapChainRes().pSwapChain->getHandle(),
			.pImageIndices = &mImageIndex,
			.pResults = NULL
		};

		VkResult res = VK_ERROR_OUT_OF_DATE_KHR;

		const auto& queue = GetCurrentContext()->getDeviceRes().mDeviceQueues.Present;
		res = vkQueuePresentKHR(queue->getHandle(), &PresentInfo);

		m_FrameIndex = (m_FrameIndex + 1) % m_NumImages;

		WaitIdle(*queue);

		return res;
	}
}