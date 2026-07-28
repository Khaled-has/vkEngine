#include "Editor.h"

#include "TitleBar/TitleBar.h"
#include "Label/Label.h"
#include "Label/Inspector.h"

#include <imgui.h>
#include <UI.h>

namespace Editor
{

    void Editor::Initialize(Engine::EngineCore* pCurrentEngine)
    {
        pEngine = pCurrentEngine;
        // # UI context
        UI::SetUIContext(reinterpret_cast<UI::UIContext*>(pCurrentEngine->getGPUContext()));
        // # ImGui context
        ImGui::SetCurrentContext(reinterpret_cast<ImGuiContext*>(pCurrentEngine->getImGuiContext()));
        // # Scene manager context
        Engine::SceneManager::SetCurrent(pCurrentEngine->getSceneContext());
        Engine::SceneManager::AddNewScene("test_scene");
        Engine::SceneManager::SelectActiveScene(0);

        // # Setup ImGui style
        SetupImGuiStyle();

        pCurrentEngine->PushLayer(new TitleBar());
        Label* pLabel = new Label();
        pCurrentEngine->PushLayer(pLabel);
        pCurrentEngine->PushLayer(new Inspector(pLabel->getSelectedIndex()));
    }

    void Editor::SetupImGuiStyle()
    {
        ImGuiIO& io = ImGui::GetIO();

        io.Fonts->AddFontFromFileTTF((std::string(RES_PATH) + "JetBrainsMonoNL-ExtraBold.ttf").c_str());

        // --
        ImGui::StyleColorsDark();
        ImGuiStyle& style = ImGui::GetStyle();

        // Font Style
        style.FontSizeBase = 22.0f;


        // Colors
        style.Colors[ImGuiCol_Button].w = 0.f;
        style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0, 0, 0, 1.f);
        style.Colors[ImGuiCol_WindowBg] = ImVec4(0.12549f, 0.12549f, 0.12549f, 1.f);
        style.Colors[ImGuiCol_TabDimmedSelected] = ImVec4(0.19549f, 0.19549f, 0.19549f, 1.f);
        style.Colors[ImGuiCol_Border] = ImVec4(0, 0, 0, 0);
    }

}