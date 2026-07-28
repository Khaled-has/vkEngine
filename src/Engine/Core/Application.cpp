#include "Application.h"

#include "Timer.h"

#include <imgui.h>
#include <UI.h>

#include "Layer.h"

namespace Engine
{

	Application::Application(ApplicationInfo mInfo)
		: EngineCore(mInfo)
	{
#ifdef WIN32
		// # Create windows window
		m_pAppWindow = CreateWindowsWindow({ .mTitle = mInfo.mProjName, .mWidth = 1440, .mHeight = 720 });
#endif
		// # Create the window's components
		m_pAppWindow->Create();
		// # Set the event callback function
		m_pAppWindow->SetEventCallback(std::bind(&Application::OnEvent, this, std::placeholders::_1));

		// # Create the renderer
		m_pRenderer = CreateRenderer();
		m_pRenderer->Initialize();

		// # create GPU context
		UI::CreateContext();

		// # Create layer stack
		m_pLayerStack = new LayerStack();

		// # Create scene manager
		m_pSceneManager = new SceneManager();

		// # Time start
		Timer::Start();
	}

	void Application::Run()
	{
		// # Layer stack setup
		m_pLayerStack->Setup();

		// # Application main loop
		while (m_AppRunning)
		{
			// # Delta time
			static double current = Timer::getTime();
			static double last = Timer::getTime();

			current = Timer::getTime();
			double deltaTime = current - last;
			last = current;

			// # 1: Update the application's window
			m_pAppWindow->OnUpdate();

			// # 2: Update the application's layers
			m_pLayerStack->OnUpdate(deltaTime);

			// # 3: LayerStack: ImGui render
			m_pLayerStack->OnImGuiRender();

			// # 4: Update the application's renderer
			m_pRenderer->Rendering();
		}
	}

	void Application::Destroy()
	{
		// # Destroy application's components
		m_pLayerStack->Destroy();
		m_pRenderer->Destroy();
		m_pAppWindow->Exit();
	}

	void* Application::getImGuiContext()
	{
		return ImGui::GetCurrentContext();
	}

	void* Application::getGPUContext()
	{
		return UI::GetUIContext();
	}

	SceneManager* Application::getSceneContext()
	{
		return m_pSceneManager;
	}

	void Application::PushLayer(Layer* pLayer)
	{
		m_pLayerStack->PushLayer_(pLayer);
	}

	void Application::CloseApplication()
	{
		m_AppRunning = false;
	}

	void Application::MaximizeApplication()
	{
		m_pAppWindow->Maximize();
	}

	void Application::MinimizeApplication()
	{
		m_pAppWindow->Minimize();
	}

	void Application::OnEvent(Event* event)
	{
		EventDispatcher dispatcher(*event);
		// # 1: Check if application closed
		dispatcher.Dispatch<WindowCloseEvent>(EVENT_BIND_FUNC(Application::OnWindowClose));
		// # 2: Check if application's window is resized
		dispatcher.Dispatch<WindowResizeEvent>(EVENT_BIND_FUNC(Application::OnWindowResize));
		// # 3: Check if keyboard pressed
		dispatcher.Dispatch<KeyPressedEvent>(EVENT_BIND_FUNC(Application::OnKeyPressed));

		// # Layer stack events
		m_pLayerStack->OnEvent(*event);
	}
	bool Application::OnKeyPressed(KeyPressedEvent& event)
	{
		std::cout << "KeyCode: " << event.GetKeyCode() << std::endl;
		if (event.GetKeyCode() == 27) m_AppRunning = false;
		return false;
	}
	bool Application::OnWindowResize(WindowResizeEvent& event)
	{
		std::cout << "New width: " << event.GetWidth() << " | New height: " << event.GetHeight() << std::endl;
		return true;
	}
	bool Application::OnWindowClose(WindowCloseEvent& event)
	{
		m_AppRunning = false;
		return true;
	}

}