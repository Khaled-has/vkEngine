#include "Entity.h"

Entity::Entity(entt::entity mHandle, Scene *pScene)
{
    m_EntityHandle = mHandle;
    m_pScene = pScene;
}
