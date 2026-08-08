cmake_minimum_required(VERSION 3.26)

project(vkEngine)

if(MSVC) 
	add_compile_options("/utf-8")
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")

set(EXTERN_DIR "${CMAKE_SOURCE_DIR}/extern")

add_compile_definitions(RES_PATH="${CMAKE_SOURCE_DIR}/res/")
add_compile_definitions(PROJ_PATH="${CMAKE_SOURCE_DIR}")

# === SDL3
if(WIN32)
	list(APPEND CMAKE_PREFIX_PATH "${EXTERN_DIR}/SDL3")
	find_package(SDL3 REQUIRED CONFIG COMPONENTS SDL3)
endif()

# === Executables >|<
# == Editor
set(EDITOR_EXE "Editor")
add_executable("${EDITOR_EXE}" "src/Entries/EditorEntry.cpp")

target_link_libraries("${EDITOR_EXE}" 
	PRIVATE 
		vkCore		# Engine
		vkEditor	# Editor
)

if (WIN32) # SDL3.dll for windows platform

	add_custom_command(TARGET ${EDITOR_EXE} POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_if_different
		"${EXTERN_DIR}/SDL3/lib/x64/SDL3.dll"
			$<TARGET_FILE_DIR:${EDITOR_EXE}>
	)

endif()

# == Game


# == Subdirectories == #
add_subdirectory("src/Engine/Core")				# Core
add_subdirectory("src/Engine/Platforms")		# Platforms
add_subdirectory("src/Engine/Renderer")			# Renderer
add_subdirectory("src/Engine/GPU")				# GPU
add_subdirectory("src/Engine/Event")			# Event
add_subdirectory("src/Engine/AssetsSystem")		# AssetsSystem
add_subdirectory("src/Engine/Scene")			# Scene
#add_subdirectory("src/Engine/UI")				# UI

#add_subdirectory("src/Editor")					# Editor

add_subdirectory("${EXTERN_DIR}/ImGui") 		# ImGui
add_subdirectory("${EXTERN_DIR}/volk")			# volk