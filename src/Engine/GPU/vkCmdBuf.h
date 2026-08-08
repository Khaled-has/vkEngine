#ifndef VKCMDBUF_H
#define VKCMDBUF_H

#include "vkConfig.h"

namespace GPU
{
	class VKGPU_API vkCmdBuf
	{
		friend class vkCmdPool;
	public:
		vkCmdBuf() {}

		inline const VkCommandBuffer& getHandle() const { return m_CmdBuf; }

		inline bool IsUnInited()	const { return m_State == CmdState::UN_INITED ? true : false; }
		inline bool IsReady()		const { return m_State == CmdState::ON_READY ? true : false; }
		inline bool IsBegin()		const { return m_State == CmdState::ON_BEGIN ? true : false; }
		inline bool IsEnded()		const { return m_State == CmdState::ON_ENDED ? true : false; }
		inline bool IsSubmited()	const { return m_State == CmdState::ON_SUBMITED ? true : false; }
		
	private:
		inline void SetUnInited()	{ m_State = CmdState::UN_INITED; }
		inline void SetReady()		{ m_State = CmdState::ON_READY; }
		inline void SetBegin()		{ m_State = CmdState::ON_BEGIN; }
		inline void SetEnded()		{ m_State = CmdState::ON_ENDED; }
		inline void SetSubmited()   { m_State = CmdState::ON_SUBMITED; }

	private:
		VkCommandBuffer m_CmdBuf = VK_NULL_HANDLE;

		enum class CmdState
		{
			UN_INITED,
			ON_READY,
			ON_BEGIN,
			ON_ENDED,
			ON_SUBMITED
		};
		CmdState m_State = CmdState::UN_INITED;
	};

	class VKGPU_API vkCmdPool
	{
	public:
		vkCmdPool() {}

		void Create(uint32_t mQFamilyIndex);
		void Destroy();

		inline const VkCommandPool& getHandle() const { return m_CmdPool; }

		void AllocCmdBufs(std::span<vkCmdBuf> mCmdBufs) const;
		void FreeCmdBufs(std::span<vkCmdBuf> mCmdBufs) const;

	private:
		VkCommandPool m_CmdPool = VK_NULL_HANDLE;
	};

}

#endif