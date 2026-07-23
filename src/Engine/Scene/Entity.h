#pragma once

#include "Scene.h"
#include "Components.h"

class Entity
{
public:
    Entity() = default;
    Entity(entt::entity mHandle, Scene* pScene);
    Entity(const Entity&) = default;

    template<typename T, typename... Args>
    T& AddComponent(Args&&... args)
    {
        if (HasComponent<T>()) 
        { 
            std::cout << "The entity has this compnent before" << std::endl;
            exit(1);
        }
        T& component = m_pScene->m_Registry.emplace<T>(m_EntityHandle, std::forward(args)...);
        return component;
    }

    template<typename T>
    void RemoveEntity()
    {
        if (!HasComponent<T>())
        {
            std::cout << "The entity hasn't this compnent" << std::endl;
            exit(1);
        }
        m_pScene->m_Registry.remove<T>(m_EntityHandle);
    }

    template<typename T>
    bool HasComponent()
    {
        return m_pScene->m_Registry.all_of<T>(m_EntityHandle);
    }

    operator bool() const { return m_EntityHandle != entt::null; }
    operator entt::entity() const { return m_EntityHandle; }
    operator uint32_t() const { return (uint32_t)m_EntityHandle; }

private:
    entt::entity m_EntityHandle = entt::null;
    Scene* m_pScene = nullptr;
};
