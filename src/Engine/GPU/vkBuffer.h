#ifndef VKBUFFER_H
#define VKBUFFER_H

#include "vkCmdBuf.h"

namespace GPU
{
	class VKGPU_API vkBuffer
	{
	public:
		vkBuffer() {}

		void Create(VkBufferCreateInfo mBufferInfo, VmaAllocationCreateInfo mAllocInfo);
		void Destroy();

		inline const VkBuffer& getBuffer() const { return m_Buffer; }
		inline const VmaAllocation& getAllocation() const { return m_Allocation; }
		inline const VmaAllocationInfo& getAllocInfo() const { return m_AllocInfo; }

		inline VkDeviceSize getBufferSize() const { return m_BufferSize; }

	private:
		VkBuffer m_Buffer			= VK_NULL_HANDLE;
		VmaAllocation m_Allocation	= VK_NULL_HANDLE;
		VkDeviceSize m_BufferSize	= 0;
		VmaAllocationInfo m_AllocInfo;
	};

	VKGPU_API void CopyBufferToBuffer(const vkBuffer& mSrc, const vkBuffer& mDst, VkBufferCopy mRegion, vkCmdBuf& mCmdBuf);
}

#endif