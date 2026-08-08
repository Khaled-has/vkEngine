#include "vkBuffer.h"
#include "vkContext.h"
#include "vkLog.h"

namespace GPU
{
	void vkBuffer::Create(VkBufferCreateInfo mBufferInfo, VmaAllocationCreateInfo mAllocInfo)
	{
		CheckContext("Create vkBuffer");

		GPU_CHECK(
			vmaCreateBuffer(GetCurrentContext()->getVmaRes().mAllocator, &mBufferInfo, &mAllocInfo, &m_Buffer, &m_Allocation, &m_AllocInfo),
			"vmaCreateBuffer"
		);
	}

	void vkBuffer::Destroy()
	{
		vmaDestroyBuffer(GetCurrentContext()->getVmaRes().mAllocator, m_Buffer, m_Allocation);
	}

	void CopyBufferToBuffer(const vkBuffer& mSrc, const vkBuffer& mDst, VkBufferCopy mRegion)
	{
		//vkCmdCopyBuffer(, vertexStageBuffer.getBuffer(), m_pVertexBuffer.getBuffer(), 1, &copyRegion);
		//vkEndCommandBuffer(mCopyCmdBuf.getHandle());

		//mDevice.getQueues().mTransfer->SubmitSync(&mCopyCmdBuf.getHandle(), 1);
		//vkQueueWaitIdle(mDevice.getQueues().mTransfer->getHandle());
	}

}