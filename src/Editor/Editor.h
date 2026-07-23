#pragma once

#include "EditorAPI.h"

namespace Editor
{

    class VK_EDITOR_API Editor
    {
    public:
        Editor() {}

        // # Init & Create the editor's layers
        void Initialize(void* pImContext);

    private:
        void SetupImGuiStyle();
    };

}