#include "Scene.h"

#include "Entity.h"

namespace Engine
{
	void Scene::AddNewEntity(const char* pName)
	{
		Entity en(m_Registry.create(), this);

		en.AddComponent<TagComponent>(pName);
		en.AddComponent<TransformComponent>(
			glm::vec3(0.0f, 0.0f, 0.0f), 
			glm::vec3(1.0f, 1.0f, 1.0f),
			glm::vec3(0.0f, 0.0f, 0.0f)
		);
	}
}