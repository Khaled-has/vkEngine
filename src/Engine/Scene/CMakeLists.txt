
add_library(vkScene)

target_sources(vkScene 
    PRIVATE
        "Scene.cpp" "Entity.cpp"
    PUBLIC 
    FILE_SET HEADERS
    BASE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}"
    FILES
        "SceneManager.h" "Scene.h" "Entity.h" "Components.h"
)

target_include_directories(vkScene 
    PUBLIC
        "${EXTERN_DIR}/entt"
        "${EXTERN_DIR}/glm"
)
