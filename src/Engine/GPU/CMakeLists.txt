
add_library(vkGPU)

target_sources(vkGPU
	PRIVATE
		"vkContext.cpp" "vkCreations.cpp" "vkDevice.cpp" "vkPhysDevice.cpp"
		"vkQueue.cpp" "vkSwapChain.cpp" "vkCmdBuf.cpp" "vkGraphicsPipeline.cpp"
		"vkShader.cpp" "vkTexture.cpp" "vkBuffer.cpp" "vkSemaphore.cpp"
	PUBLIC
	FILE_SET HEADERS
	BASE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}"
	FILES
		"vkGPU.h"
)

target_include_directories(vkGPU PUBLIC
	"${EXTERN_DIR}/vma"
	"${EXTERN_DIR}/shaderc"
)

target_link_libraries(vkGPU 
	PRIVATE
		"${EXTERN_DIR}/shaderc/shaderc_shared.lib"
		vkCoreInterface
	PUBLIC
		volk
)