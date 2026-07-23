
add_library(vkUI)

target_sources(vkUI
	PRIVATE
	"Texture.cpp" "ITexture.cpp"
	PUBLIC 
    FILE_SET HEADERS
    BASE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}"
    FILES
	"Texture.h"
)

target_link_libraries(vkUI
	PRIVATE
		vkGPU
		ImGui_Core
)

target_include_directories(vkUI
	PRIVATE
		"${EXTERN_DIR}/stb"
)

add_compile_definitions(vkUI PRIVATE ENGINE_BUILD)