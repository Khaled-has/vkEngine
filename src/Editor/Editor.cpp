#include "Editor.h"

#include "TitleBar/TitleBar.h"

#include <imgui.h>

namespace Editor
{

    void Editor::Initialize(void* pImContext)
    {
        ImGui::SetCurrentContext(reinterpret_cast<ImGuiContext*>(pImContext));
        SetupImGuiStyle();

        Engine::PushLayer(new TitleBar());
    }

    void Editor::SetupImGuiStyle()
    {
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