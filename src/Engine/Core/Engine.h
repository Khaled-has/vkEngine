#pragma once

#include <functional>

#include "EngineAPI.h"
#include "Layer.h"

#include <SceneManager.h>
#include <Renderer.h>

namespace Engine
{

	struct EngineContext
	{
		ImGuiContext* pImContext;
		SceneManager* pSceneContext;
		Render::Renderer* pRendererContext;
	};

	struct EngineInfo
	{
		std::string mProjName;
		std::string mProjPath;
	};

	class VK_ENGINE_API EngineCore
	{
	public:
		EngineCore(EngineInfo mInfo) : mAppInfo(mInfo) {}
		~EngineCore() = default;

		static void SetCurrent(EngineCore* pCurrent);

		static inline void PushLayer(Layer* pLayer) { pInstance->PushLayerImpl(pLayer); }

		static inline void CloseApplication() { pInstance->CloseApplicationImpl(); }
		static inline void MaximizeApplication() { pInstance->MaximizeApplicationImpl(); }
		static inline void MinimizeApplication() { pInstance->MinimizeApplicationImpl(); }

		virtual void Run() = 0;
		virtual void Destroy() = 0;

		virtual void PushLayerImpl(Layer* pLayer) = 0;

		virtual void CloseApplicationImpl() = 0;
		virtual void MaximizeApplicationImpl() = 0;
		virtual void MinimizeApplicationImpl() = 0;

		static inline EngineInfo getAppInfo() { return pInstance->mAppInfo; }
		static inline EngineContext getContext() { return pInstance->m_pAppContext; }
	protected:
		static EngineCore* pInstance;

		EngineContext m_pAppContext;
		EngineInfo mAppInfo;
	};

	extern "C" VK_ENGINE_API EngineCore* Create_Engine(EngineInfo mInfo);
}