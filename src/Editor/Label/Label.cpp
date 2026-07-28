#include "Label.h"

#include <imgui.h>

void Label::Setup()
{
	Engine::Scene& scene = Engine::SceneManager::getActiveScene();
	scene.AddNewEntity("Object Triangle");
}

void Label::Destroy()
{

}

void Label::OnImGuiRender()
{
	ImGui::Begin("Label", nullptr);

	for (auto& En : Engine::SceneManager::getActiveScene().getAllEntities())
	{
		Engine::Entity entity(En, &Engine::SceneManager::getActiveScene());
		DrawEntity(entity);
	}

	ImGui::End();
}

void Label::DrawEntity(Engine::Entity& entity)
{
	auto& tag = entity.GetComponent<Engine::TagComponent>();

	ImGuiTreeNodeFlags flags = 
		((m_SelectedEntt == (uint32_t)entity) ? ImGuiTreeNodeFlags_Selected : 0)
		| ImGuiTreeNodeFlags_OpenOnArrow;
	flags |= ImGuiTreeNodeFlags_SpanAvailWidth;


	void* ptrID = (void*)(uint64_t)(uint32_t)entity;

	bool opened = ImGui::TreeNodeEx(ptrID, flags, "%s", tag.mTag.c_str());

	if (ImGui::IsItemClicked())
	{
		m_SelectedEntt = (uint32_t)entity;
	}

	if (opened)
		ImGui::TreePop();
}
