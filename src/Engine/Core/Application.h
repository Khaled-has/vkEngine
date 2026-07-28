#pragma once

#include <iostream>
#include <functional>

#include "Window.h"
#include "Renderer.h"

#include "Event.h"
#include "KeyEvent.h"
#include "ApplicationEvent.h"

#include "LayerStack.h"

#include "Engine.h"

namespace Engine
{

	class VK_ENGINE_API Application : public EngineCore
	{
	public:
		Application(ApplicationInfo mInfo);
		~Application() {}

		virtual void Run() override;
		virtual void Destroy() override;

		virtual void* getImGuiContext() override;
		virtual void* getGPUContext() override;

		virtual SceneManager* getSceneContext() override;

		virtual void PushLayer(Layer* pLayer) override;

		virtual void CloseApplication() override;
		virtual void MaximizeApplication() override;
		virtual void MinimizeApplication() override;

	private:
		Window* m_pAppWindow;
		Render::Renderer* m_pRenderer;

		LayerStack* m_pLayerStack;
		SceneManager* m_pSceneManager;

		bool m_AppRunning = true;

		void OnEvent(Event* event);

		bool OnKeyPressed(KeyPressedEvent& event);

		bool OnWindowResize(WindowResizeEvent& event);
		bool OnWindowClose(WindowCloseEvent& event);
	};

}