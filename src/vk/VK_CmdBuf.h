#ifndef VK_CMDBUF_H
#define VK_CMDBUF_H

#include <volk.h>

class VK_CmdBuf
{
	friend class VK_CmdPool;
public:
	VK_CmdBuf() {}

	inline const VkCommandBuffer& getHandle() const { return m_pCmdBuf; }

	inline void SetIs_Ready() { m_pState = CmdState::ON_READY; }
	inline void SetIs_Begin() { m_pState = CmdState::ON_BEGIN; }
	inline void SetIs_Render_Pass() { m_pState = CmdState::ON_RENDER_PASS; }
	inline void SetIs_Ended() { m_pState = CmdState::ON_ENDED; }
	inline void SetIs_Submited() { m_pState = CmdState::ON_SUBMITED; }

	inline bool IsReady() const { return m_pState == CmdState::ON_READY ? true : false; }
	inline bool IsBegin() const { return m_pState == CmdState::ON_BEGIN ? true : false; }
	inline bool IsRender_Pass() const { return m_pState == CmdState::ON_RENDER_PASS ? true : false; }
	inline bool IsEnded() const { return m_pState == CmdState::ON_ENDED ? true : false; }
	inline bool IsSubmited() const { return m_pState == CmdState::ON_SUBMITED ? true : false; }

private:
	VkCommandBuffer m_pCmdBuf = VK_NULL_HANDLE;

	enum class CmdState
	{
		ON_READY,
		ON_BEGIN,
		ON_RENDER_PASS,
		ON_ENDED,
		ON_SUBMITED
	};
	CmdState m_pState = CmdState::ON_READY;
};

class VK_CmdPool
{
public:
	VK_CmdPool() {}
	 
	void Create(uint32_t mQFamilyIndex);
	void Destroy();

	inline const VkCommandPool& getHandle() const { return m_pCmdPool; }

	void AllocCmdBufs(uint32_t mCount, VK_CmdBuf* pCmdBufs);
	void FreeCmdBufs(uint32_t mCount, VK_CmdBuf* pCmdBufs);

private:
	VkCommandPool m_pCmdPool = VK_NULL_HANDLE;
};

void BeginCmdBuf(const VK_CmdBuf& mCmdBuf, VkCommandBufferUsageFlags mUsageFlags);

#endif