#include "VK_CmdBuf.h"

#include "config.h"
#include "VK_Device.h"

#include <vector>

void VK_CmdPool::Create(uint32_t mQFamilyIndex)
{
	VkCommandPoolCreateInfo poolInfo = {
		.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
		.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
		.queueFamilyIndex = mQFamilyIndex
	};

	CHECK_VK_RES(vkCreateCommandPool(VK_Device::Get()->getDevice(), &poolInfo, NULL, &m_pCmdPool), "vkCreateCommandPool");
}

void VK_CmdPool::Destroy()
{
	vkDestroyCommandPool(VK_Device::Get()->getDevice(), m_pCmdPool, NULL);
}

void VK_CmdPool::AllocCmdBufs(uint32_t mCount, VK_CmdBuf* pCmdBufs) const
{
	VkCommandBufferAllocateInfo allocInfo = {
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
		.commandPool = m_pCmdPool,
		.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
		.commandBufferCount = mCount
	};

	std::vector<VkCommandBuffer> Cmds(mCount);

	CHECK_VK_RES(vkAllocateCommandBuffers(VK_Device::Get()->getDevice(), &allocInfo, Cmds.data()), "vkAllocateCommandBuffers");

	for (uint32_t i = 0; i < mCount; i++)
	{
		pCmdBufs[i].m_pCmdBuf = Cmds[i];
	}
}

void VK_CmdPool::FreeCmdBufs(uint32_t mCount, VK_CmdBuf* pCmdBufs) const
{
	std::vector<VkCommandBuffer> Cmds;
	for (uint32_t i = 0; i < mCount; i++)
	{
		Cmds.push_back(pCmdBufs[i].m_pCmdBuf);
	}

	vkFreeCommandBuffers(VK_Device::Get()->getDevice(), m_pCmdPool, mCount, Cmds.data());
}

void BeginCmdBuf(const VK_CmdBuf& mCmdBuf, VkCommandBufferUsageFlags mUsageFlags)
{
	VkCommandBufferBeginInfo Info = {
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
		.pNext = NULL,
		.flags = mUsageFlags,
		.pInheritanceInfo = NULL
	};

	CHECK_VK_RES(vkBeginCommandBuffer(mCmdBuf.getHandle(), &Info), "vkBeginCommandBuffer");
}
