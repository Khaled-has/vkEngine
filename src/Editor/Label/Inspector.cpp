#include "Inspector.h"

#include <SceneManager.h>

void Inspector::Setup()
{

}

void Inspector::Destroy()
{

}

void Inspector::OnImGuiRender()
{
	ImGui::Begin("Inspector");

	if (*m_pSelectedItem != -1)
	{
		auto& scene = Engine::SceneManager::getActiveScene();

		uint32_t id = *m_pSelectedItem;
		Engine::Entity entt(static_cast<entt::entity>(id), &scene);

		mTransformUI.DrawEntt(entt);
	}

	ImGui::End();
}
