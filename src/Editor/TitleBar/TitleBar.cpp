#include "TitleBar.h"

#include <imgui.h>

void TitleBar::Setup()
{
	//UI::CreateTexture((std::string(RES_PATH) + "icon.png").c_str(), mTestTex);
}

void TitleBar::Destroy()
{
}

void TitleBar::OnUpdate(double mDelta)
{
}

void TitleBar::OnEvent(Event &event)
{
}

void TitleBar::OnImGuiRender()
{
   ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(8.f, 15.f));

	if (ImGui::BeginMainMenuBar())
	{	
		ImGui::Spacing();
		ImGui::Image(mTestTex->ImTextureID(), ImVec2(50, 50));
		ImGui::Spacing();

		ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 8.0f);
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("FFF"))
			{
				ImGui::EndTabItem();
			}

			ImGui::EndMenu();
		}

		ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 8.0f);
		if (ImGui::BeginMenu("Edit"))
		{
			if (ImGui::MenuItem("FFF"))
			{
				ImGui::EndTabItem();
			}

			ImGui::EndMenu();
		}

		ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 8.0f);
		if (ImGui::BeginMenu("Window"))
		{
			if (ImGui::MenuItem("FFF"))
			{
				ImGui::EndTabItem();
			}

			ImGui::EndMenu();
		}

		// Title
		ImGui::SameLine(((ImGui::GetWindowWidth() / 2) - 125.0f));
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 8.0f);
		//ImGui::Text(w_Prop.Title.c_str());

		/* ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 8.0f); */
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 250.0f);
		if (ImGui::BeginMenu("Development"))
		{
			if (ImGui::MenuItem("FFF"))
			{
				ImGui::EndTabItem();
			}

			ImGui::EndMenu();
		}

		// ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 8.0f);
		// ImGui::SameLine(ImGui::GetWindowWidth() - 45.f);

		// if (ImGui::ImageButton("EXIE", (ImTextureRef)textureBar.GetID(), ImVec2(20.f, 20.f), ImVec2(0, 0), ImVec2(0.3333f, 1.0f)))
		// {
		// 	// Close The Application
		// 	//*pRunning = false;
		// }

		// ImGui::SameLine(ImGui::GetWindowWidth() - 84.f);
		// if (ImGui::ImageButton("MAXMIZE", (ImTextureRef)textureBar.GetID(), ImVec2(20.f, 20.f), ImVec2(0.3333f, 0.0f), ImVec2(0.6666f, 1.0f)))
		// {
		// 	static bool max = false;
		// 	if (!max)
		// 	{
		// 		SetWin32Show(w_Window, SW_MAXIMIZE);
		// 		max = true;
		// 	}
		// 	else
		// 	{
		// 		SetWin32Show(w_Window, SW_SHOWDEFAULT);
		// 		max = false;
		// 	}
		// }

		ImGui::SameLine(ImGui::GetWindowWidth() - 124.f);
		// if (ImGui::ImageButton("MIXMIZE", (ImTextureRef)textureBar.GetID(), ImVec2(20.f, 20.f), ImVec2(0.6666f, 0.0f), ImVec2(1.0f, 1.0f)))
		// {
		// 	SetWin32Show(w_Window, SW_MINIMIZE);
		// }

	}
	ImGui::EndMainMenuBar();
	ImGui::PopStyleVar();

	if (ImGui::Begin("Editor Style"))
		ImGui::ShowStyleEditor(&ImGui::GetStyle());
    ImGui::End();
}
