#ifndef VKQUEUE_H
#define VKQUEUE_H

#include "vkConfig.h"

namespace GPU
{
	class VKGPU_API vkQueue
	{
		friend class vkDevice;
	public:
		vkQueue() {}

		inline const VkQueue& getHandle() const { return m_Queue; }

		inline uint32_t getQueueIndex() const { return m_QueueIndex; }
		inline uint32_t getQFamilyIndex() const { return m_QFamilyIndex; }

	private:
		VkQueue m_Queue = VK_NULL_HANDLE;
		uint32_t m_QueueIndex = 0;
		uint32_t m_QFamilyIndex = 0;

		void CreateQueue(uint32_t mQFamilyIndex, uint32_t mQIndex);
	};

	typedef struct vkDeviceQueues {
		vkQueue* Graphics;
		vkQueue* Transfer;
		vkQueue* Compute;
		vkQueue* Present;
	} vkDeviceQueues;

	VKGPU_API void SubmitSync(const vkQueue& mQueue);
	VKGPU_API void WaitIdle(const vkQueue& mQueue);
}

#endif