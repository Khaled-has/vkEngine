
add_library(vkEditor SHARED)

target_sources(vkEditor
    PRIVATE
        "Editor.cpp" "TitleBar/TitleBar.cpp" "Label/Label.cpp" "${EXTERN_DIR}/stb/stb_image.cpp"
        "Label/Inspector.cpp" "Label/UIComponents/TransformUI.cpp"
    PUBLIC
	FILE_SET HEADERS
	BASE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}"
	FILES
        "EditorAPI.h" "Editor.h"
)

target_include_directories(vkEditor PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")

target_link_libraries(vkEditor 
    PRIVATE
       vkCore
       vkRenderer
       vkUI
       vkScene
       volk
       SDL3::SDL3
    PUBLIC
       ImGui_UI
)

add_compile_definitions(PRIVATE EDITOR_BUILD)