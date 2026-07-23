#pragma once

#include <iostream>

#include <entt/entity/entity.hpp>
#include <entt/entity/registry.hpp>
#include <entt/entity/component.hpp>

class Scene
{
    friend class Entity;
public:
    Scene(const char* mName) : m_name(mName) {}
    inline std::string getName() const { return m_name; }

    template<typename... Args>
    inline auto getEntities()
    {
        return m_Registry.view<Args...>();
    }
private:
    std::string m_name = "Empty";

    entt::registry m_Registry;
};