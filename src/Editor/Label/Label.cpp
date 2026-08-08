#include "Label.h"

#include <imgui.h>
#include <Renderer.h>

void Label::Setup()
{
	Engine::Scene& scene = Engine::SceneManager::getActiveScene();

	scene.AddNewEntity("Object Triangle");

	m_Viewport = Render::Create_Viewport();
	m_Viewport->Create({ .mViewSize{1440, 720}, .pScene = nullptr, .pCamEntity = nullptr });
	Render::Renderer::AddViewport(m_Viewport);
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

	ImGui::Begin("Viewport-Test");

	ImGui::Image(m_Viewport->getImageView(), ImGui::GetWindowSize());

	ImGui::End();
}

void Label::DrawEntity(Engine::Entity& entity)
{
	auto& tag = entity.GetComponent<Engine::TagComponent>();

	ImGuiTreeNodeFlags flags = 
		((m_SelectedEntt == (uint32_t)entity) ? ImGuiTreeNodeFlags_Selected : 0)
		| ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;

	void* ptrID = (void*)(uint64_t)(uint32_t)entity;

	bool opened = ImGui::TreeNodeEx(ptrID, flags, "%s", tag.mTag.c_str());

	if (ImGui::IsItemClicked())
	{
		m_SelectedEntt = (uint32_t)entity;
	}

	if (opened)
		ImGui::TreePop();
}
