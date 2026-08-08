#pragma once

#include <Engine.h>

#include "EditorAPI.h"

namespace Editor
{

    struct TitleBarSerializer
    {
        bool mFileMenu = false;
        bool mEditeMenu = false;
    };

    class VK_EDITOR_API Editor
    {
    public:
        Editor() { pInstance = this; }

        static inline TitleBarSerializer& getTitleBarSer() { return pInstance->mTitleBarSer; }

        // # Init & Create the editor's layers
        void Initialize(Engine::EngineCore* pCurrentEngine);

    private:
        static inline Editor* pInstance;

        TitleBarSerializer mTitleBarSer;

        void SetupImGuiStyle();

    };

}