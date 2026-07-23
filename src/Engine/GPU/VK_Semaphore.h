#ifndef VK_SEMAPHORE_H
#define VK_SEMAPHORE_H

#include "VK_SwapChain.h"
#include "VK_CmdBuf.h"
#include "VK_Queue.h"

struct VK_Semaphore
{
	VkSemaphore mRenderFinishedSem;
	VkSemaphore mImageAvailableSem;
	VkFence mInFlightFence;
	VkFence mImageInFlightFence;
};

class VK_SemaphoresManager
{
public:
	VK_SemaphoresManager() {}

	void Create(uint32_t mFrameCount);
	void Destroy();

	VkResult AcquireNextImage(const VK_SwapChain& mSwapChain);
	void SubmitAsync(const VK_CmdBuf* CmdBuf, uint32_t Count, const VK_Queue& mQueue);
	VkResult Present(uint32_t mImageIndex, const VK_SwapChain& mSwapChain, const VK_Queue& mQueue);

	void WaitIdle(const VkQueue& mQueue);


	inline uint32_t getAcquiredImageIndex() { return mAcquiredImageIndex; }
	inline uint32_t getFrameIndex() { return mFrameIndex; }
private:
	std::vector<VK_Semaphore> m_Semaphores;

	uint32_t mNumImages = 0;
	uint32_t mFrameIndex = 0;
	uint32_t mAcquiredImageIndex = 0;
};

#endif