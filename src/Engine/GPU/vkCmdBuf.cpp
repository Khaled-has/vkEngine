#include "vkCmdBuf.h"
#include "vkContext.h"
#include "vkLog.h"

namespace GPU
{
	void vkCmdPool::Create(uint32_t mQFamilyIndex)
	{
		CheckContext("Create vkCmdPool");
		if (!GetCurrentContext()->getDeviceRes().mInitialized)
		{
			GPU_LOG_ERROR("vkGPU: Create the vkDevice before you trying to create vkCmdPool");
			exit(1);
		}

		VkCommandPoolCreateInfo poolInfo = {
			.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
			.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
			.queueFamilyIndex = mQFamilyIndex
		};

		GPU_CHECK(
			vkCreateCommandPool(GetCurrentContext()->getDeviceRes().pDevice->getHandle(), &poolInfo, NULL, &m_CmdPool),
			"vkCreateCommandPool"
		);
	}

	void vkCmdPool::Destroy()
	{
		vkDestroyCommandPool(GetCurrentContext()->getDeviceRes().pDevice->getHandle(), m_CmdPool, NULL);
	}

	void vkCmdPool::AllocCmdBufs(std::span<vkCmdBuf> mCmdBufs) const
	{
		VkCommandBufferAllocateInfo allocInfo = {
			.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
			.commandPool = m_CmdPool,
			.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
			.commandBufferCount = (uint32_t)mCmdBufs.size()
		};

		std::vector<VkCommandBuffer> Cmds(mCmdBufs.size());

		GPU_CHECK(
			vkAllocateCommandBuffers(GetCurrentContext()->getDeviceRes().pDevice->getHandle(), &allocInfo, Cmds.data()),
			"vkAllocateCommandBuffers"
		);

		for (uint32_t i = 0; i < mCmdBufs.size(); i++)
		{
			// # Check the cmd's state
			if (!mCmdBufs[i].IsUnInited())
			{
				GPU_LOG_ERROR("vkGPU: You'r trying to allocate vkCmdBuf is already allocated");
				exit(1);
			}
			// # Create the cmd and changing his state
			mCmdBufs[i].m_CmdBuf = Cmds[i];
			mCmdBufs[i].SetReady();
		}
	}

	void vkCmdPool::FreeCmdBufs(std::span<vkCmdBuf> mCmdBufs) const
	{
		std::vector<VkCommandBuffer> Cmds(mCmdBufs.size());

		for (uint32_t i = 0; i < mCmdBufs.size(); i++)
		{
			// # Check the cmd's state
			if (mCmdBufs[i].IsUnInited())
			{
				GPU_LOG_ERROR("vkGPU: You'r trying to free vkCmdBuf is uninitialized");
				exit(1);
			} 
			// # Add the cmd & changing his state
			Cmds[i] = mCmdBufs[i].m_CmdBuf;
			mCmdBufs[i].SetUnInited();
		}

		// # Free the cmd
		vkFreeCommandBuffers(
			GetCurrentContext()->getDeviceRes().pDevice->getHandle(), 
			m_CmdPool, (uint32_t)mCmdBufs.size(),
			Cmds.data()
		);
	}
}