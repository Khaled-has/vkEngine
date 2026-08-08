#ifndef VKSEMAPHORE_H
#define VKSEMAPHORE_H

#include "vkConfig.h"
#include "vkCmdBuf.h"

namespace GPU
{
	struct vkSemaphore
	{
		VkSemaphore mRenderFinishedSem;
		VkSemaphore mImageAvailableSem;
		VkFence mInFlightFence;
		VkFence mImageInFlightFence;
	};

	class vkSemaphoreManager
	{
	public:
		VK_SemaphoreManager() {}

		void Create();
		void Destroy();

		VkResult AcquireNextImage();
		void SubmitAsync(std::span<const vkCmdBuf> mCmdBufs);
		VkResult Present(uint32_t mImageIndex);

		inline uint32_t getAcquiredImageIndex() { return mAcquiredImageIndex; }
		inline uint32_t getFrameIndex()			{ return mFrameIndex; }
	private:
		std::vector<vkSemaphore> m_Semaphores;

		uint32_t m_NumImages = 0;
		uint32_t m_FrameIndex = 0;
		uint32_t m_AcquiredImageIndex = 0;
	};
}

#endif