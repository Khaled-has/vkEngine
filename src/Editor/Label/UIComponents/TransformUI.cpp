#include "TransformUI.h"

#include <imgui_internal.h>

static void DrawVec3Control(const std::string& label, glm::vec3& values, float restValue = 0.0f, float columnWidth = 120.0f)
{
	ImGuiIO& io = ImGui::GetIO();

	ImGui::PushID(label.c_str());

	ImGui::Columns(2);
	ImGui::SetColumnWidth(0, columnWidth);
	ImGui::Text("%s", label.c_str());
	ImGui::NextColumn();

	ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));

	float lineHeight = ImGui::GetStyle().FontSizeBase + GImGui->Style.FramePadding.y * 2.0f;
	ImVec2 buttonSize = ImVec2(lineHeight + 3.0f, lineHeight);

	// == X axis
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.1f, 0.15f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.9f, 0.2f, 0.2f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.8f, 0.1f, 0.15f, 1.0f));

	if (ImGui::Button("X", buttonSize))
		values.x = restValue;

	ImGui::PopStyleColor(3);

	ImGui::SameLine();
	ImGui::DragFloat("##X", &values.x, 0.1f, 0.0f, 0.0f, "%.2f");
	ImGui::PopItemWidth();
	ImGui::SameLine();

	// == Y axis
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.7f, 0.2f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.8f, 0.3f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.2f, 0.7f, 0.2f, 1.0f));

	if (ImGui::Button("Y", buttonSize))
		values.y = restValue;

	ImGui::PopStyleColor(3);

	ImGui::SameLine();
	ImGui::DragFloat("##Y", &values.y, 0.1f, 0.0f, 0.0f, "%.2f");
	ImGui::PopItemWidth();
	ImGui::SameLine();

	// == Z axis
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.1f, 0.25f, 0.8f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.2f, 0.35f, 0.9f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.1f, 0.25f, 0.8f, 1.0f));

	if (ImGui::Button("Z", buttonSize))
		values.z = restValue;

	ImGui::PopStyleColor(3);

	ImGui::SameLine();
	ImGui::DragFloat("##Z", &values.z, 0.1f, 0.0f, 0.0f, "%.2f");
	ImGui::PopItemWidth();

	// ===
	ImGui::PopStyleVar();
	ImGui::Columns(1);

	ImGui::PopID();
}

namespace UIComponent
{
	void TransformUI::DrawEntt(Engine::Entity& entt)
	{
		if (entt.HasComponent<Engine::TransformComponent>())
		{
			auto& co = entt.GetComponent<Engine::TransformComponent>();

			DrawVec3Control("Translation", co.mPosition);
			ImGui::Spacing();
			DrawVec3Control("Scale", co.mScale);
			ImGui::Spacing();
			DrawVec3Control("Rotation", co.mRotation);
		}
	}
}