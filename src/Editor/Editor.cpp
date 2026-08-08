#include "Editor.h"

#include "TitleBar/TitleBar.h"
#include "Label/Label.h"
#include "Label/Inspector.h"

#include <imgui.h>
#include <UI.h>
#include <Renderer.h>

namespace Editor
{
    static void InitEngineSystems()
    {
        Engine::EngineContext ctx = Engine::EngineCore::getContext();

        ImGui::SetCurrentContext(ctx.pImContext);
        UI::SetUIContext(ctx.pUIContext);
        Engine::SceneManager::SetCurrent(ctx.pSceneContext);
        Render::Renderer::SetCurrent(ctx.pRendererContext);
    }

    void Editor::Initialize(Engine::EngineCore* pCurrentEngine)
    {
        Engine::EngineCore::SetCurrent(pCurrentEngine);
        InitEngineSystems();

        Engine::SceneManager::AddNewScene("test_scene");
        Engine::SceneManager::SelectActiveScene(0);

        // # Setup ImGui style
        SetupImGuiStyle();

        Engine::EngineCore::PushLayer(new TitleBar());
        Label* pLabel = new Label();
        Engine::EngineCore::PushLayer(pLabel);
        Engine::EngineCore::PushLayer(new Inspector(pLabel->getSelectedIndex()));
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