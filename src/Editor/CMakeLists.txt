
add_library(vkEditor SHARED)

target_sources(vkEditor
    PRIVATE
        "Editor.cpp" "TitleBar/TitleBar.cpp"
    PUBLIC
	FILE_SET HEADERS
	BASE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}"
	FILES
        "Editor.h"
)

target_link_libraries(vkEditor 
    PRIVATE
       vkCore
       ImGui_Core
)

add_compile_definitions(PRIVATE EDITOR_BUILD)