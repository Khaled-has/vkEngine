#pragma once

#include <vector>

#include "Scene.h"

namespace Engine
{
    class SceneManager
    {
    public:
        SceneManager() {}

        inline void Create() { m_pInstance = this; }
        static inline void SetCurrent(SceneManager* pCurrent) { m_pInstance = pCurrent; }

        static inline void AddNewScene(const char* pName) { m_pInstance->m_Scenes.push_back(Scene(pName)); }
        static inline std::vector<Scene>& getScenes() { m_pInstance->m_Scenes; }
        static inline void SelectActiveScene(uint32_t mHandle) { m_pInstance->m_SelectedScene = mHandle; }
        static inline Scene& getActiveScene()
        {
            if (m_pInstance->m_SelectedScene == -1)
            {
                std::cout << "Select active scene before you call: getActiveScene function" << std::endl;
                exit(1);
            }
            return m_pInstance->m_Scenes[m_pInstance->m_SelectedScene];
        }
    private:
        static inline SceneManager* m_pInstance;

        std::vector<Scene> m_Scenes;
        int m_SelectedScene = -1;
    };
}