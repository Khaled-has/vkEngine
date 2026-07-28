#pragma once

#include <functional>

#include "EngineAPI.h"
#include "Layer.h"

#include <SceneManager.h>

namespace Engine
{
	struct ApplicationInfo
	{
		std::string mProjName;
		std::string mProjPath;
	};

	class VK_ENGINE_API EngineCore
	{
	public:
		EngineCore(ApplicationInfo mInfo) : mAppInfo(mInfo) {}
		~EngineCore() = default;

		virtual void Run() = 0;
		virtual void Destroy() = 0;

		virtual void* getImGuiContext() = 0;
		virtual void* getGPUContext() = 0;

		virtual SceneManager* getSceneContext() = 0;

		virtual void PushLayer(Layer* pLayer) = 0;

		virtual void CloseApplication() = 0;
		virtual void MaximizeApplication() = 0;
		virtual void MinimizeApplication() = 0;

		inline const ApplicationInfo& getAppInfo() const { return mAppInfo; }

	protected:
		ApplicationInfo mAppInfo;
	};

	extern "C" VK_ENGINE_API EngineCore* Create_Engine(ApplicationInfo mInfo);
}