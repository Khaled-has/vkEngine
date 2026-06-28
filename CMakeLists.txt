cmake_minimum_required(VERSION 3.26)

project(vkEngine)

if(MSVC) 
	add_compile_options("/utf-8")
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# == GLFW3
list(APPEND CMAKE_PREFIX_PATH "${CMAKE_SOURCE_DIR}/extern/SDL3/cmake")
find_package(SDL3 CONFIG REQUIRED)

set(SDL3_DIR "${CMAKE_SOURCE_DIR}/extern/SDL3")

# == vulkan
set(VK_DIR "${CMAKE_SOURCE_DIR}/extern/vulkan")
set(VOLK_DIR "${CMAKE_SOURCE_DIR}/extern/volk")
set(VMA_DIR "${CMAKE_SOURCE_DIR}/extern/vma")

set(SHADERC "${CMAKE_SOURCE_DIR}/extern/shaderc")

add_executable(${PROJECT_NAME})

target_sources(${PROJECT_NAME} PRIVATE
	"${VOLK_DIR}/volk.c"
	"src/main.cpp" "src/vk/VK_Device.cpp" "src/vk/log.cpp" "src/vk/VK_SwapChain.cpp" 
	"src/vk/VK_Queue.cpp" "src/vk/VK_Texture.cpp" "src/vk/VK_CmdBuf.cpp" "src/vk/VK_GraphPipeline.cpp" 
	"src/vk/VK_RenderPass.cpp" "src/renderer/renderer.cpp" "src/vk/VK_Semaphore.cpp" "src/vk/VK_Buffer.cpp" "src/vk/VK_Shader.cpp"
)

# == Include directories
target_include_directories(${PROJECT_NAME} PRIVATE
	"${VK_DIR}/include"			# vulkan
	"${VOLK_DIR}"				# volk
	"${VMA_DIR}"				# vma

	"${SDL3_DIR}/include"		# SDL3
	"${SHADERC}"				# shaderc
	"${CMAKE_SOURCE_DIR}/extern/spdlog/include"	# spdlog

	"${CMAKE_SOURCE_DIR}/src"
)

# == Link librarise
target_link_libraries(${PROJECT_NAME} PRIVATE
	SDL3::SDL3
	"${VK_DIR}/lib/vulkan-1.lib"		# vulkan
	"${SHADERC}/shaderc_shared.lib"		# shaderc
)

if(WIN32)

	add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
						COMMAND ${CMAKE_COMMAND} -E copy_if_different
						"${SDL3_DIR}/lib/x64/SDL3.dll"
							$<TARGET_FILE_DIR:${PROJECT_NAME}>
	)

endif()

add_compile_definitions(RES_PATH="${CMAKE_SOURCE_DIR}/res/")