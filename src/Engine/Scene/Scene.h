#pragma once

#include <iostream>

#include <entt/entity/entity.hpp>
#include <entt/entity/registry.hpp>
#include <entt/entity/component.hpp>

namespace Engine
{
    class Scene
    {
        friend class Entity;
    public:
        Scene(const char* mName) : m_name(mName) {}
        inline std::string getName() const { return m_name; }

        template<typename T>
        inline entt::view<T>& getEntities() { return m_Registry.view<T>(); }

        inline entt::storage<entt::entity>& getAllEntities() { return m_Registry.storage<entt::entity>(); }

        void AddNewEntity(const char* pName = "Entity");

    private:
        std::string m_name = "Empty";

        entt::registry m_Registry;
    };
}