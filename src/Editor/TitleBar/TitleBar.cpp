#include "TitleBar.h"
#include "Editor.h"

#include <imgui.h>

void TitleBar::Setup()
{
	mIconTex.load((std::string(RES_PATH) + "icon.png").c_str());
	mTBarTex.load((std::string(RES_PATH) + "bar.png").c_str());
}

void TitleBar::Destroy()
{
	mIconTex.destroy();
	mTBarTex.destroy();
}

void TitleBar::OnUpdate(double mDelta)
{
}

void TitleBar::OnEvent(Event& event)
{
}

void TitleBar::OnImGuiRender()
{
	CustumTBar();

	if (ImGui::Begin("EditorStyle", nullptr))
	{
		ImGui::ShowStyleEditor();
	}
	ImGui::End();

	ImGui::Begin("TestMenu");
	ImGui::End();
}

void TitleBar::CustumTBar()
{
	// Custum window bar
	ImGuiWindowFlags flags =
		ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings |
		ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse
		| ImGuiWindowFlags_NoDocking;

	ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(viewport->WorkPos);
	ImGui::SetNextWindowSize(ImVec2(viewport->WorkSize.x, 55.0f));

	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0, 0, 0, 0));
	ImGui::Begin("###CustumTitleBar", nullptr, flags);
	ImGui::PopStyleColor();

	ImDrawList* drawList = ImGui::GetWindowDrawList();

	ImVec2 min = ImGui::GetWindowPos();
	ImVec2 max = ImVec2(min.x + 280.0f, min.y + 55.0f);

	ImU32 colorLeft = ImColor(240, 80, 80, 255);
	ImU32 colorRight = ImColor(0, 0, 0, 255);

	drawList->AddRectFilledMultiColor(min, max, colorLeft, colorRight, colorRight, colorLeft);
	drawList->AddRectFilledMultiColor(ImVec2(max.x, 0.0f), ImVec2(ImGui::GetWindowSize().x + min.x, ImGui::GetWindowSize().y + min.y), colorRight, colorRight, colorRight, colorRight);

	// # 1 : App icon
	ImGui::SetCursorPos(ImVec2(10.0f, 6.0f));
	ImGui::Image((ImTextureID)mIconTex, ImVec2(52.f, 40.f));

	// # 2 : Items

	// > File
	ImGui::SameLine();
	ImGui::SetCursorPos(ImVec2(90.0f, 0.0f));
	if (ImGui::Button("File"))
	{
		FileMenu();
	}

	// > Edite
	ImGui::SameLine();
	ImGui::SetCursorPosY(0.0f);
	if (ImGui::Button("Edite"))
	{
		EditeMenu();
	}

	// > View
	ImGui::SameLine();
	ImGui::SetCursorPosY(0.0f);
	if (ImGui::Button("View"))
	{
	
	}

	// > Tools
	ImGui::SameLine();
	ImGui::SetCursorPosY(0.0f);
	if (ImGui::Button("Tools"))
	{

	}

	// # 3 : The application title
	ImGui::SameLine();
	ImGui::SetCursorPos(ImVec2((ImGui::GetWindowSize().x / 2.0f) - 50.0f, 5.0f));
	ImGui::Text("vkEngine Hello world from vulkan");

	// # 4 : Window buttons
	WindowButtons();

	ImGui::End();
}

void TitleBar::FileMenu()
{
}

void TitleBar::EditeMenu()
{
}

void TitleBar::WindowButtons()
{
	// # Close button
	ImGui::SameLine();
	ImGui::SetCursorPos(ImVec2(ImGui::GetWindowSize().x - 38.0f, 5.0f));
	if (ImGui::ImageButton("#Exite", (ImTextureID)mTBarTex, ImVec2(25.0f, 25.0f), ImVec2(0, 0), ImVec2(1.0f / 3.0f, 1.0f)))
	{
		Editor::Editor::getEngine()->CloseApplication();
	}
	// # Maxmize button
	ImGui::SameLine();
	ImGui::SetCursorPos(ImVec2(ImGui::GetWindowSize().x - 75.0f, 5.0f));
	if (ImGui::ImageButton("#Maxmize", (ImTextureID)mTBarTex, ImVec2(25.0f, 25.0f), ImVec2(1.0f / 3.0f, 0), ImVec2(2.0f / 3.0f, 1.0f)))
	{
		Editor::Editor::getEngine()->MaximizeApplication();
	}
	// # Minmize button
	ImGui::SameLine();
	ImGui::SetCursorPos(ImVec2(ImGui::GetWindowSize().x - 112.0f, 5.0f));
	if (ImGui::ImageButton("#Minmize", (ImTextureID)mTBarTex, ImVec2(25.0f, 25.0f), ImVec2(2.0f / 3.0f, 0), ImVec2(1.0f, 1.0f)))
	{
		Editor::Editor::getEngine()->MinimizeApplication();
	}
}
