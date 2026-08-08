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

#include <imgui.h>
namespace Engine
{

	class VK_ENGINE_API Application : public EngineCore
	{
	public:
		Application(EngineInfo mInfo);
		~Application() {}

		virtual void Run() override;
		virtual void Destroy() override;

		virtual void PushLayerImpl(Layer* pLayer) override;

		virtual void CloseApplicationImpl() override;
		virtual void MaximizeApplicationImpl() override;
		virtual void MinimizeApplicationImpl() override;

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